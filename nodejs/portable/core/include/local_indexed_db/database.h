#ifndef LOCAL_INDEXED_DB_DATABASE_H_
#define LOCAL_INDEXED_DB_DATABASE_H_

#include "local_indexed_db/storage.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace local_indexed_db {

class Database {
 public:
  explicit Database(std::unique_ptr<IStorage> storage);
  ~Database();

  // Basic KV ops.
  Status Put(std::string_view key, std::string_view value);
  Status Get(std::string_view key, std::string* value);
  Status Delete(std::string_view key);

  // Atomic batch commit. If value is nullopt then delete.
  Status CommitBatch(const std::vector<std::pair<std::string, std::optional<std::string>>>& batch);

 private:
  std::unique_ptr<IStorage> storage_;
};

}  // namespace local_indexed_db

#endif  // LOCAL_INDEXED_DB_DATABASE_H_
