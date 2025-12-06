#include "local_indexed_db/database.h"
#include "local_indexed_db/encoding.h"
#include "local_indexed_db/storage.h"

#include <cassert>
#include <iostream>

using namespace local_indexed_db;

int main() {
  auto storage = CreateInMemoryStorage();
  Database db(std::move(storage));

  // Use encoding helpers to produce a key and value.
  std::string key;
  IDBKey k(std::string("mykey"));
  EncodeIDBKey(k, &key);
  std::string value = "payload";

  auto s = db.Put(key, value);
  assert(s == Status::kOk);

  std::string read;
  s = db.Get(key, &read);
  assert(s == Status::kOk);
  assert(read == value);

  // Batch commit: overwrite and delete
  std::vector<std::pair<std::string, std::optional<std::string>>> batch;
  batch.emplace_back(key, std::optional<std::string>(std::string("new")));
  batch.emplace_back(std::string("other"), std::nullopt);
  s = db.CommitBatch(batch);
  assert(s == Status::kOk);

  std::string read2;
  s = db.Get(key, &read2);
  assert(s == Status::kOk && read2 == "new");

  std::cout << "Database tests passed\n";
  return 0;
}
