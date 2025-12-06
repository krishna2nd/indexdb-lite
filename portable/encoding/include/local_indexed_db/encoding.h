// Minimal portable encoding utilities for a local IndexedDB-style library.
// This is a focused subset port of encoding helpers to bootstrap Phase A.
// Provides: Encode/Decode for byte, bool, int (simple little-endian var),
// string-with-length (UTF-8), binary, and double. Also a tiny IDBKey stub
// that supports Number and String for EncodeIDBKey/DecodeIDBKey round-trips.

#ifndef LOCAL_INDEXED_DB_ENCODING_H_
#define LOCAL_INDEXED_DB_ENCODING_H_

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace local_indexed_db {

// IDBKey representation supporting a subset of IndexedDB key types used in
// the portable encoder: Invalid, Null, Number, Date, String, Binary, Array.
// This is not a full Blink replacement but sufficient for encoder testing.
class IDBKey {
 public:
  enum class Type { kInvalid, kNull, kNumber, kDate, kString, kBinary, kArray };
  IDBKey() : type_(Type::kInvalid), number_(0.0) {}
  static IDBKey Null() { IDBKey k; k.type_ = Type::kNull; return k; }
  explicit IDBKey(double v) : type_(Type::kNumber), number_(v) {}
  // Date stored as ms since epoch
  static IDBKey Date(double ms) { IDBKey k; k.type_ = Type::kDate; k.number_ = ms; return k; }
  explicit IDBKey(std::string s) : type_(Type::kString), str_(std::move(s)) {}
  explicit IDBKey(std::vector<uint8_t> b) : type_(Type::kBinary), bin_(std::move(b)) {}
  explicit IDBKey(std::vector<IDBKey> a) : type_(Type::kArray), arr_(std::move(a)) {}

  Type type() const { return type_; }
  double number() const { return number_; }
  const std::string& str() const { return str_; }
  const std::vector<uint8_t>& binary() const { return bin_; }
  const std::vector<IDBKey>& array() const { return arr_; }

 private:
  Type type_;
  double number_ = 0.0;
  std::string str_;
  std::vector<uint8_t> bin_;
  std::vector<IDBKey> arr_;
};

// Basic encoders/decoders.
void EncodeByte(unsigned char value, std::string* into);
bool DecodeByte(std::string_view* slice, unsigned char* value);

void EncodeBool(bool value, std::string* into);
bool DecodeBool(std::string_view* slice, bool* value);

// A simple little-endian variable-size integer encoder. This "dumb" encoder
// writes little-endian 8-byte signed integer (fixed width) for simplicity.
void EncodeInt(int64_t value, std::string* into);
bool DecodeInt(std::string_view* slice, int64_t* value);

// Strings encoded as: 8-byte length (little endian) followed by UTF-8 bytes.
void EncodeStringWithLength(const std::string& value, std::string* into);
bool DecodeStringWithLength(std::string_view* slice, std::string* value);

// Binary encoding as length-prefixed blob.
void EncodeBinary(const std::string& value, std::string* into);
bool DecodeBinary(std::string_view* slice, std::string* value);

// Double encoded as 8 bytes (IEEE-754) in big-endian for sortable behavior.
void EncodeDouble(double value, std::string* into);
bool DecodeDouble(std::string_view* slice, double* value);

// Encode/Decode IDBKey for Number and String only. Decode returns an
// invalid key on failure.
void EncodeIDBKey(const IDBKey& value, std::string* into);
IDBKey DecodeIDBKey(std::string_view* slice);

// Sortable encoders: create a byte string that preserves IDB key ordering
// when compared lexicographically. Not guaranteed to be Chromium-identical
// but intended to be compatible for portable use. These functions are
// reversible via DecodeSortableIDBKey.
std::string EncodeSortableIDBKey(const IDBKey& value);
IDBKey DecodeSortableIDBKey(std::string_view serialized);

}  // namespace local_indexed_db

#endif  // LOCAL_INDEXED_DB_ENCODING_H_
