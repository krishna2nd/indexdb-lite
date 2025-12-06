#include "local_indexed_db/encoding.h"

#include <cassert>
#include <iostream>
#include <vector>

using namespace local_indexed_db;

static void TestSortableRoundTrip() {
  IDBKey n(42.5);
  IDBKey d = IDBKey::Date(1609459200000.0); // 2021-01-01T00:00:00Z in ms
  IDBKey s(std::string("abc"));
  IDBKey b(std::vector<uint8_t>{0,1,2});
  IDBKey arr(std::vector<IDBKey>{n, s, IDBKey::Null()});

  auto enc_n = EncodeSortableIDBKey(n);
  auto enc_d = EncodeSortableIDBKey(d);
  auto enc_s = EncodeSortableIDBKey(s);
  auto enc_b = EncodeSortableIDBKey(b);
  auto enc_arr = EncodeSortableIDBKey(arr);

  IDBKey dn = DecodeSortableIDBKey(enc_n);
  IDBKey dd = DecodeSortableIDBKey(enc_d);
  IDBKey ds = DecodeSortableIDBKey(enc_s);
  IDBKey db = DecodeSortableIDBKey(enc_b);
  IDBKey darr = DecodeSortableIDBKey(enc_arr);

  assert(dn.type() == IDBKey::Type::kNumber && std::abs(dn.number() - 42.5) < 1e-12);
  assert(dd.type() == IDBKey::Type::kDate);
  assert(ds.type() == IDBKey::Type::kString && ds.str() == "abc");
  assert(db.type() == IDBKey::Type::kBinary && db.binary().size() == 3 && db.binary()[1] == 1);
  assert(darr.type() == IDBKey::Type::kArray && darr.array().size() == 3);
}

int main() {
  TestSortableRoundTrip();
  std::cout << "Sortable key round-trip tests passed\n";
  return 0;
}
