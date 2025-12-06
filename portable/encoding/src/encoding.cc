#include "local_indexed_db/encoding.h"

#include <cassert>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <variant>
#include <vector>
#include <algorithm>
#include <cmath>

namespace local_indexed_db {

void EncodeByte(unsigned char value, std::string* into) {
  into->push_back(static_cast<char>(value));
}

bool DecodeByte(std::string_view* slice, unsigned char* value) {
  if (slice->empty())
    return false;
  *value = static_cast<unsigned char>(slice->front());
  slice->remove_prefix(1);
  return true;
}

void EncodeBool(bool value, std::string* into) {
  into->push_back(value ? 1 : 0);
}

bool DecodeBool(std::string_view* slice, bool* value) {
  if (slice->empty())
    return false;
  unsigned char b = static_cast<unsigned char>(slice->front());
  *value = (b != 0);
  slice->remove_prefix(1);
  return true;
}

static void AppendLittleEndian64(uint64_t v, std::string* into) {
  for (int i = 0; i < 8; ++i) {
    into->push_back(static_cast<char>(v & 0xff));
    v >>= 8;
  }
}

static bool ConsumeLittleEndian64(std::string_view* slice, uint64_t* out) {
  if (slice->size() < 8)
    return false;
  uint64_t v = 0;
  // Little endian read
  for (int i = 0; i < 8; ++i) {
    v |= (static_cast<uint64_t>(static_cast<unsigned char>(slice->at(i))) << (8 * i));
  }
  slice->remove_prefix(8);
  *out = v;
  return true;
}

void EncodeInt(int64_t value, std::string* into) {
  // Fixed-width 8-byte little-endian representation.
  uint64_t v = static_cast<uint64_t>(value);
  AppendLittleEndian64(v, into);
}

bool DecodeInt(std::string_view* slice, int64_t* value) {
  uint64_t v = 0;
  if (!ConsumeLittleEndian64(slice, &v))
    return false;
  *value = static_cast<int64_t>(v);
  return true;
}

void EncodeStringWithLength(const std::string& value, std::string* into) {
  AppendLittleEndian64(static_cast<uint64_t>(value.size()), into);
  into->append(value);
}

bool DecodeStringWithLength(std::string_view* slice, std::string* value) {
  uint64_t len = 0;
  if (!ConsumeLittleEndian64(slice, &len))
    return false;
  if (slice->size() < len)
    return false;
  value->assign(slice->data(), static_cast<size_t>(len));
  slice->remove_prefix(static_cast<size_t>(len));
  return true;
}

void EncodeBinary(const std::string& value, std::string* into) {
  EncodeStringWithLength(value, into);
}

bool DecodeBinary(std::string_view* slice, std::string* value) {
  return DecodeStringWithLength(slice, value);
}

void EncodeDouble(double value, std::string* into) {
  static_assert(sizeof(double) == 8, "double must be 8 bytes");
  uint64_t bits = 0;
  std::memcpy(&bits, &value, sizeof(bits));
  AppendLittleEndian64(bits, into);
}

bool DecodeDouble(std::string_view* slice, double* value) {
  uint64_t bits = 0;
  if (!ConsumeLittleEndian64(slice, &bits))
    return false;
  std::memcpy(value, &bits, sizeof(bits));
  return true;
}

void EncodeIDBKey(const IDBKey& value, std::string* into) {
  // Simple encoding: type byte, then payload.
  switch (value.type()) {
    case IDBKey::Type::kNull: {
      EncodeByte(0x00, into);
      return;
    }
    case IDBKey::Type::kNumber: {
      EncodeByte(0x03, into);
      EncodeDouble(value.number(), into);
      return;
    }
    case IDBKey::Type::kDate: {
      EncodeByte(0x02, into);
      EncodeDouble(value.number(), into);
      return;
    }
    case IDBKey::Type::kString: {
      EncodeByte(0x01, into);
      EncodeStringWithLength(value.str(), into);
      return;
    }
    case IDBKey::Type::kBinary: {
      EncodeByte(0x06, into);
      // Binary as length-prefixed
      EncodeBinary(std::string(reinterpret_cast<const char*>(value.binary().data()), value.binary().size()), into);
      return;
    }
    case IDBKey::Type::kArray: {
      EncodeByte(0x04, into);
      // Encode each member recursively, then a terminator 0xff
      for (const auto& member : value.array()) {
        EncodeIDBKey(member, into);
      }
      EncodeByte(0xff, into);
      return;
    }
    default:
      EncodeByte(0x05, into); // invalid
      return;
  }
}

IDBKey DecodeIDBKey(std::string_view* slice) {
  if (slice->empty())
    return IDBKey();
  unsigned char t = static_cast<unsigned char>(slice->front());
  slice->remove_prefix(1);
  switch (t) {
    case 0x00:
      return IDBKey::Null();
    case 0x03: {
      double d = 0;
      if (!DecodeDouble(slice, &d))
        return IDBKey();
      return IDBKey(d);
    }
    case 0x02: {
      double d = 0;
      if (!DecodeDouble(slice, &d))
        return IDBKey();
      return IDBKey::Date(d);
    }
    case 0x01: {
      std::string s;
      if (!DecodeStringWithLength(slice, &s))
        return IDBKey();
      return IDBKey(std::move(s));
    }
    case 0x06: {
      std::string b;
      if (!DecodeBinary(slice, &b))
        return IDBKey();
      std::vector<uint8_t> vb(b.begin(), b.end());
      return IDBKey(std::move(vb));
    }
    case 0x04: {
      std::vector<IDBKey> arr;
      while (!slice->empty()) {
        if (static_cast<unsigned char>(slice->front()) == 0xff) {
          slice->remove_prefix(1);
          break;
        }
        arr.push_back(DecodeIDBKey(slice));
      }
      return IDBKey(std::move(arr));
    }
    default:
      return IDBKey();
  }
}

// Sortable double helpers modeled after Chromium's transform.
static void EncodeSortableDouble(double value, std::string* into) {
  // Disallow NaN for keys.
  if (std::isnan(value)) throw std::runtime_error("NaN not supported");
  uint64_t bits = 0;
  std::memcpy(&bits, &value, sizeof(bits));
  uint64_t modified_bits;
  const uint64_t kSignBit = uint64_t{1} << 63;
  if (std::signbit(value) && value != -0.0) {
    modified_bits = bits ^ std::numeric_limits<uint64_t>::max();
  } else {
    modified_bits = bits | kSignBit;
  }
  // Append big-endian so lexical comparison works.
  for (int i = 7; i >= 0; --i) {
    into->push_back(static_cast<char>((modified_bits >> (8 * i)) & 0xff));
  }
}

static bool DecodeSortableDouble(std::string_view& slice, double* output) {
  if (slice.size() < 8) return false;
  uint64_t modified_bits = 0;
  for (int i = 0; i < 8; ++i) {
    modified_bits = (modified_bits << 8) | static_cast<unsigned char>(slice[i]);
  }
  slice.remove_prefix(8);
  const uint64_t kSignBit = uint64_t{1} << 63;
  uint64_t bits;
  if (modified_bits & kSignBit) {
    bits = modified_bits ^ kSignBit;
  } else {
    bits = modified_bits ^ std::numeric_limits<uint64_t>::max();
  }
  std::memcpy(output, &bits, sizeof(bits));
  if (std::isnan(*output) || (std::signbit(*output) && *output == -0.0)) return false;
  return true;
}

std::string EncodeSortableIDBKey(const IDBKey& value) {
  std::string out;
  // Prefix a type byte that encodes ordering intent.
  switch (value.type()) {
    case IDBKey::Type::kNull:
      out.push_back(0x00);
      break;
    case IDBKey::Type::kNumber:
      out.push_back(0x10);
      EncodeSortableDouble(value.number(), &out);
      break;
    case IDBKey::Type::kDate:
      out.push_back(0x20);
      EncodeSortableDouble(value.number(), &out);
      break;
    case IDBKey::Type::kString:
      out.push_back(0x30);
      // simple: length-prefixed UTF-8 (not sentinel-based)
      AppendLittleEndian64(static_cast<uint64_t>(value.str().size()), &out);
      out.append(value.str());
      break;
    case IDBKey::Type::kBinary:
      out.push_back(0x40);
      AppendLittleEndian64(static_cast<uint64_t>(value.binary().size()), &out);
      out.append(reinterpret_cast<const char*>(value.binary().data()), value.binary().size());
      break;
    case IDBKey::Type::kArray:
      out.push_back(0x50);
      for (const auto& m : value.array()) {
        std::string member = EncodeSortableIDBKey(m);
        AppendLittleEndian64(member.size(), &out);
        out.append(member);
      }
      // terminator length 0
      AppendLittleEndian64(0, &out);
      break;
    default:
      out.push_back(0xff);
      break;
  }
  return out;
}

IDBKey DecodeSortableIDBKey(std::string_view serialized) {
  if (serialized.empty()) return IDBKey();
  unsigned char t = static_cast<unsigned char>(serialized.front());
  serialized.remove_prefix(1);
  switch (t) {
    case 0x00:
      return IDBKey::Null();
    case 0x10: {
      double d = 0;
      if (!DecodeSortableDouble(serialized, &d)) return IDBKey();
      return IDBKey(d);
    }
    case 0x20: {
      double d = 0;
      if (!DecodeSortableDouble(serialized, &d)) return IDBKey();
      return IDBKey::Date(d);
    }
    case 0x30: {
      uint64_t len = 0;
      if (!ConsumeLittleEndian64(&serialized, &len)) return IDBKey();
      if (serialized.size() < len) return IDBKey();
      std::string s(serialized.data(), static_cast<size_t>(len));
      serialized.remove_prefix(static_cast<size_t>(len));
      return IDBKey(std::move(s));
    }
    case 0x40: {
      uint64_t len = 0;
      if (!ConsumeLittleEndian64(&serialized, &len)) return IDBKey();
      if (serialized.size() < len) return IDBKey();
      std::vector<uint8_t> b(serialized.data(), serialized.data() + len);
      serialized.remove_prefix(static_cast<size_t>(len));
      return IDBKey(std::move(b));
    }
    case 0x50: {
      std::vector<IDBKey> arr;
      while (true) {
        uint64_t mlen = 0;
        if (!ConsumeLittleEndian64(&serialized, &mlen)) return IDBKey();
        if (mlen == 0) break;
        if (serialized.size() < mlen) return IDBKey();
        std::string_view member_sv(serialized.data(), static_cast<size_t>(mlen));
        arr.push_back(DecodeSortableIDBKey(member_sv));
        serialized.remove_prefix(static_cast<size_t>(mlen));
      }
      return IDBKey(std::move(arr));
    }
    default:
      return IDBKey();
  }
}

}  // namespace local_indexed_db
