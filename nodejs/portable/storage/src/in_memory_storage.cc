#include "local_indexed_db/storage.h"

#include <map>
#include <mutex>
#include <optional>

namespace local_indexed_db {

class InMemoryIterator : public Iterator {
 public:
  InMemoryIterator(std::map<std::string, std::string>::const_iterator it,
                   std::map<std::string, std::string>::const_iterator end)
      : it_(it), end_(end) {}
  bool Valid() const override { return it_ != end_; }
  void Next() override { if (it_ != end_) ++it_; }
  std::string_view Key() const override { return it_->first; }
  std::string_view Value() const override { return it_->second; }
 private:
  std::map<std::string, std::string>::const_iterator it_, end_;
};

class InMemoryStorage : public IStorage {
 public:
  Status Put(std::string_view key, std::string_view value) override {
    std::lock_guard<std::mutex> lock(mu_);
    store_[std::string(key)] = std::string(value);
    return Status::kOk;
  }

  Status Get(std::string_view key, std::string* value) override {
    std::lock_guard<std::mutex> lock(mu_);
    auto it = store_.find(std::string(key));
    if (it == store_.end())
      return Status::kNotFound;
    *value = it->second;
    return Status::kOk;
  }

  Status Delete(std::string_view key) override {
    std::lock_guard<std::mutex> lock(mu_);
    auto it = store_.find(std::string(key));
    if (it == store_.end())
      return Status::kNotFound;
    store_.erase(it);
    return Status::kOk;
  }

  Status WriteBatch(const std::vector<std::pair<std::string, std::optional<std::string>>>& batch) override {
    std::lock_guard<std::mutex> lock(mu_);
    for (const auto& kv : batch) {
      if (kv.second.has_value())
        store_[kv.first] = *kv.second;
      else
        store_.erase(kv.first);
    }
    return Status::kOk;
  }

 private:
  std::map<std::string, std::string> store_;
  std::mutex mu_;
};

std::unique_ptr<IStorage> CreateInMemoryStorage() {
  return std::unique_ptr<IStorage>(new InMemoryStorage());
}

}  // namespace local_indexed_db
