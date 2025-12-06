#include "local_indexed_db/encoding.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace local_indexed_db;

static void TestByteBool() {
  std::string buf;
  EncodeByte(0x7f, &buf);
  EncodeBool(true, &buf);
  std::string_view sv(buf);
  unsigned char b = 0;
  bool bo = false;
  bool ok = DecodeByte(&sv, &b) && DecodeBool(&sv, &bo);
  assert(ok);
  assert(b == 0x7f);
  assert(bo == true);
}

static void TestInt() {
  std::string buf;
  EncodeInt(-42, &buf);
  std::string_view sv(buf);
  int64_t v = 0;
  bool ok = DecodeInt(&sv, &v);
  assert(ok);
  assert(v == -42);
}

static void TestStringBinary() {
  std::string buf;
  EncodeStringWithLength("hello", &buf);
  EncodeBinary(std::string("\x00\x01\x02", 3), &buf);
  std::cerr << "raw buffer bytes:";
  for (unsigned char c : buf) std::cerr << " " << std::hex << int(c) << std::dec;
  std::cerr << "\n";
  std::string_view sv(buf);
  std::string s;
  std::string b;
  bool ok = DecodeStringWithLength(&sv, &s) && DecodeBinary(&sv, &b);
  assert(ok);
  std::cerr << "decoded string='" << s << "' size=" << s.size() << "\n";
  std::cerr << "decoded binary size=" << b.size() << " bytes:";
  for (size_t i = 0; i < b.size(); ++i) std::cerr << " " << int(static_cast<unsigned char>(b[i]));
  std::cerr << "\n";
  assert(s == "hello");
  assert(b.size() == 3 && b[0] == 0 && b[1] == 1 && b[2] == 2);
}

static void TestDoubleAndKey() {
  std::string buf;
  EncodeDouble(3.14159, &buf);
  IDBKey k1(2.5);
  IDBKey k2(std::string("foo"));
  EncodeIDBKey(k1, &buf);
  EncodeIDBKey(k2, &buf);

  std::string_view sv(buf);
  double d = 0;
  bool ok = DecodeDouble(&sv, &d);
  assert(ok);
  // approximate check
  assert(std::abs(d - 3.14159) < 1e-8);

  IDBKey dk1 = DecodeIDBKey(&sv);
  IDBKey dk2 = DecodeIDBKey(&sv);
  assert(dk1.type() == IDBKey::Type::kNumber);
  assert(dk2.type() == IDBKey::Type::kString);
  assert(std::abs(dk1.number() - 2.5) < 1e-12);
  assert(dk2.str() == "foo");
}

int main() {
  TestByteBool();
  TestInt();
  TestStringBinary();
  TestDoubleAndKey();
  std::cout << "All encoding tests passed\n";
  return 0;
}
