#ifndef LOCAL_INDEXED_DB_STORAGE_H_
#define LOCAL_INDEXED_DB_STORAGE_H_

#include <functional>
#include <memory>
#include <string>
#include <optional>
#include <string_view>
#include <vector>

namespace local_indexed_db {

// Simple status.
enum class Status { kOk, kNotFound, kError };

class Iterator {
 public:
  virtual ~Iterator() = default;
  virtual bool Valid() const = 0;
  virtual void Next() = 0;
  virtual std::string_view Key() const = 0;
  virtual std::string_view Value() const = 0;
};

// Storage interface: minimal key/value store used by the library.
class IStorage {
 public:
  virtual ~IStorage() = default;
  virtual Status Put(std::string_view key, std::string_view value) = 0;
  virtual Status Get(std::string_view key, std::string* value) = 0;
  virtual Status Delete(std::string_view key) = 0;
  // Atomically apply the vector of (key, value). If value is nullopt, delete.
  virtual Status WriteBatch(const std::vector<std::pair<std::string, std::optional<std::string>>>& batch) = 0;
};

// Factory for an in-memory storage suitable for tests.
std::unique_ptr<IStorage> CreateInMemoryStorage();

}  // namespace local_indexed_db

#endif  // LOCAL_INDEXED_DB_STORAGE_H_
