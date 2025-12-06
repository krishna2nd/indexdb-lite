// Example: Using the Local Indexed DB library

#include "local_indexed_db.h"
#include <iostream>

using namespace local_indexed_db;

int main() {
  std::cout << "=== Local Indexed DB Example ===\n\n";

  // 1. Create an in-memory database
  std::cout << "1. Creating in-memory database...\n";
  auto db = CreateInMemoryDatabase();

  // 2. Encode and store keys using IDBKey
  std::cout << "2. Storing encoded keys...\n";
  IDBKey string_key(std::string("product_name"));
  IDBKey number_key(42.5);
  IDBKey date_key = IDBKey::Date(1609459200000.0);

  std::string enc_string, enc_number, enc_date;
  EncodeIDBKey(string_key, &enc_string);
  EncodeIDBKey(number_key, &enc_number);
  EncodeIDBKey(date_key, &enc_date);

  auto status = db->Put(enc_string, "Laptop");
  std::cout << "  Stored string key: " << (status == Status::kOk ? "OK" : "FAILED") << "\n";

  status = db->Put(enc_number, "42.5 items");
  std::cout << "  Stored number key: " << (status == Status::kOk ? "OK" : "FAILED") << "\n";

  status = db->Put(enc_date, "2021-01-01");
  std::cout << "  Stored date key: " << (status == Status::kOk ? "OK" : "FAILED") << "\n";

  // 3. Retrieve values
  std::cout << "\n3. Retrieving values...\n";
  std::string product;
  status = db->Get(enc_string, &product);
  std::cout << "  Retrieved product: " << product << " (" << (status == Status::kOk ? "OK" : "NOT FOUND") << ")\n";

  // 4. Use sortable encoding to preserve ordering
  std::cout << "\n4. Using sortable keys (order-preserving)...\n";
  std::string sort_key1 = EncodeSortableIDBKey(IDBKey(10.0));
  std::string sort_key2 = EncodeSortableIDBKey(IDBKey(20.0));
  std::string sort_key3 = EncodeSortableIDBKey(IDBKey(5.0));
  db->Put(sort_key1, "value_10");
  db->Put(sort_key2, "value_20");
  db->Put(sort_key3, "value_5");
  std::cout << "  Stored 3 numbers with sortable encoding (10, 20, 5)\n";
  std::cout << "  Keys preserve order: " << (sort_key3 < sort_key1 && sort_key1 < sort_key2 ? "YES" : "NO") << "\n";

  // 5. Use keyspace prefixes for hierarchical storage
  std::cout << "\n5. Using keyspace prefixes...\n";
  ObjectStoreDataKey data_key(1, 2, "user_id_123");
  std::string prefixed = data_key.Encode();
  db->Put(prefixed, "User Alice");
  std::cout << "  Stored user record with prefixed key\n";

  std::string user_data;
  db->Get(prefixed, &user_data);
  std::cout << "  Retrieved: " << user_data << "\n";

  // 6. Batch transactions
  std::cout << "\n6. Performing batch transaction...\n";
  std::vector<std::pair<std::string, std::optional<std::string>>> batch;
  batch.emplace_back("batch_k1", std::optional<std::string>("batch_v1"));
  batch.emplace_back("batch_k2", std::optional<std::string>("batch_v2"));
  batch.emplace_back("batch_k3", std::optional<std::string>("batch_v3"));
  status = db->CommitBatch(batch);
  std::cout << "  Batch commit: " << (status == Status::kOk ? "OK" : "FAILED") << "\n";

  std::string batch_v2;
  db->Get("batch_k2", &batch_v2);
  std::cout << "  Retrieved batch value: " << batch_v2 << "\n";

  // 7. Complex keys (arrays)
  std::cout << "\n7. Using complex key types (arrays)...\n";
  std::vector<IDBKey> arr_members{IDBKey(1.0), IDBKey(std::string("two")), IDBKey::Null()};
  IDBKey complex_key(arr_members);
  std::string enc_complex = EncodeSortableIDBKey(complex_key);
  db->Put(enc_complex, "array_value");
  std::cout << "  Stored array key [1, 'two', null]\n";

  IDBKey decoded_complex = DecodeSortableIDBKey(enc_complex);
  std::cout << "  Retrieved array key type: " << (decoded_complex.type() == IDBKey::Type::kArray ? "kArray" : "OTHER") << "\n";

  std::cout << "\n=== Example Complete ===\n";
  return 0;
}
