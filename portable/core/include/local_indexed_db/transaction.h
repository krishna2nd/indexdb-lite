#ifndef LOCAL_INDEXED_DB_TRANSACTION_H_
#define LOCAL_INDEXED_DB_TRANSACTION_H_

#include "local_indexed_db/storage.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <optional>

namespace local_indexed_db {

// A snapshot transaction: provides a consistent view of the DB at transaction start.
class Transaction {
 public:
  explicit Transaction(std::unique_ptr<IStorage> storage);
  ~Transaction();

  Status Put(std::string_view key, std::string_view value);
  Status Get(std::string_view key, std::string* value);
  Status Delete(std::string_view key);

  // Prepare a batch of writes to be committed atomically.
  void AddWriteOp(std::string key, std::optional<std::string> value);

  // Commit all buffered writes atomically.
  Status Commit();

  // Rollback (clears write buffer, no action if not committed).
  void Rollback();

 private:
  std::unique_ptr<IStorage> storage_;
  std::vector<std::pair<std::string, std::optional<std::string>>> writes_;
};

}  // namespace local_indexed_db

#endif  // LOCAL_INDEXED_DB_TRANSACTION_H_
