#include "local_indexed_db/database.h"

namespace local_indexed_db {

Database::Database(std::unique_ptr<IStorage> storage) : storage_(std::move(storage)) {}
Database::~Database() = default;

Status Database::Put(std::string_view key, std::string_view value) {
  return storage_->Put(key, value);
}

Status Database::Get(std::string_view key, std::string* value) {
  return storage_->Get(key, value);
}

Status Database::Delete(std::string_view key) {
  return storage_->Delete(key);
}

Status Database::CommitBatch(const std::vector<std::pair<std::string, std::optional<std::string>>>& batch) {
  return storage_->WriteBatch(batch);
}

}  // namespace local_indexed_db
