#include "local_indexed_db/keyspace.h"
#include <cassert>
#include <iostream>

using namespace local_indexed_db;

int main() {
  // Test KeyPrefix encoding
  KeyPrefix db_prefix(123);
  std::string enc = db_prefix.Encode();
  assert(!enc.empty());

  KeyPrefix store_prefix(123, 456);
  enc = store_prefix.Encode();
  assert(!enc.empty());

  KeyPrefix index_prefix(123, 456, 789);
  enc = index_prefix.Encode();
  assert(!enc.empty());

  // Test ObjectStoreDataKey
  ObjectStoreDataKey data_key(123, 456, "user_key_abc");
  enc = data_key.Encode();
  assert(!enc.empty());
  assert(data_key.database_id() == 123);
  assert(data_key.object_store_id() == 456);

  // Test IndexDataKey
  IndexDataKey index_key(123, 456, 789, "idx_key", "primary_key");
  enc = index_key.Encode();
  assert(!enc.empty());
  assert(index_key.index_id() == 789);

  std::cout << "Keyspace tests passed\n";
  return 0;
}
