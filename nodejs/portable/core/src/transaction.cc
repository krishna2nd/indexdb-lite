#include "local_indexed_db/transaction.h"

namespace local_indexed_db {

Transaction::Transaction(std::unique_ptr<IStorage> storage) : storage_(std::move(storage)) {}
Transaction::~Transaction() = default;

Status Transaction::Put(std::string_view key, std::string_view value) {
  return storage_->Put(key, value);
}

Status Transaction::Get(std::string_view key, std::string* value) {
  return storage_->Get(key, value);
}

Status Transaction::Delete(std::string_view key) {
  return storage_->Delete(key);
}

void Transaction::AddWriteOp(std::string key, std::optional<std::string> value) {
  writes_.emplace_back(std::move(key), value);
}

Status Transaction::Commit() {
  auto status = storage_->WriteBatch(writes_);
  writes_.clear();
  return status;
}

void Transaction::Rollback() {
  writes_.clear();
}

}  // namespace local_indexed_db
