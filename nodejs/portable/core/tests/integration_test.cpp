#include "local_indexed_db/database.h"
#include "local_indexed_db/encoding.h"
#include "local_indexed_db/transaction.h"
#include "local_indexed_db/keyspace.h"
#include "local_indexed_db/storage.h"

#include <cassert>
#include <iostream>

using namespace local_indexed_db;

int main() {
  // Create in-memory storage and database
  auto storage = CreateInMemoryStorage();
  Database db(std::move(storage));

  // Test 1: Basic put/get
  std::string key = "test_key";
  std::string value = "test_value";
  auto status = db.Put(key, value);
  assert(status == Status::kOk);

  std::string read_value;
  status = db.Get(key, &read_value);
  assert(status == Status::kOk);
  assert(read_value == value);

  // Test 2: Keys with IDBKey encoding
  IDBKey num_key(123.45);
  std::string encoded_key;
  EncodeIDBKey(num_key, &encoded_key);
  status = db.Put(encoded_key, "numeric_value");
  assert(status == Status::kOk);

  std::string read_numeric;
  status = db.Get(encoded_key, &read_numeric);
  assert(status == Status::kOk);
  assert(read_numeric == "numeric_value");

  // Test 3: Transaction with batch commit
  auto storage2 = CreateInMemoryStorage();
  Database db2(std::move(storage2));
  
  auto txn = std::make_unique<Transaction>(CreateInMemoryStorage());
  txn->AddWriteOp("batch_key_1", std::optional<std::string>("value1"));
  txn->AddWriteOp("batch_key_2", std::optional<std::string>("value2"));
  status = txn->Commit();
  assert(status == Status::kOk);

  // Test 4: KeyPrefix usage
  KeyPrefix db_prefix(1001);
  ObjectStoreDataKey data_key(1001, 2002, "user_data");
  std::string prefixed_key = data_key.Encode();
  assert(!prefixed_key.empty());

  status = db.Put(prefixed_key, "prefixed_value");
  assert(status == Status::kOk);

  // Test 5: Delete
  status = db.Delete(key);
  assert(status == Status::kOk);

  status = db.Get(key, &read_value);
  assert(status == Status::kNotFound);

  std::cout << "Integration tests passed\n";
  return 0;
}
