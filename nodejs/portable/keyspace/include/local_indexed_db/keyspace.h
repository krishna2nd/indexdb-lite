#ifndef LOCAL_INDEXED_DB_KEYSPACE_H_
#define LOCAL_INDEXED_DB_KEYSPACE_H_

#include <cstdint>
#include <string>

namespace local_indexed_db {

// KeyPrefix: the high-level keyspace organization for IndexedDB stores.
// This encodes database_id, object_store_id, and optionally index_id to
// create a hierarchical key prefix.
class KeyPrefix {
 public:
  enum Type {
    GLOBAL_METADATA = 0,
    DATABASE_METADATA = 1,
    OBJECT_STORE_DATA = 2,
    EXISTS_ENTRY = 3,
    INDEX_DATA = 4,
    INVALID_TYPE = 5,
    BLOB_ENTRY = 6
  };

  KeyPrefix();
  explicit KeyPrefix(int64_t database_id);
  KeyPrefix(int64_t database_id, int64_t object_store_id);
  KeyPrefix(int64_t database_id, int64_t object_store_id, int64_t index_id);

  static std::string EncodeEmpty();
  std::string Encode() const;
  static bool Decode(const std::string& data, KeyPrefix* result);

  Type type() const;
  int64_t database_id() const { return database_id_; }
  int64_t object_store_id() const { return object_store_id_; }
  int64_t index_id() const { return index_id_; }

 private:
  Type type_;
  int64_t database_id_;
  int64_t object_store_id_;
  int64_t index_id_;
};

// ObjectStoreDataKey: key for actual stored data in an object store.
class ObjectStoreDataKey {
 public:
  ObjectStoreDataKey() = default;
  ObjectStoreDataKey(int64_t database_id, int64_t object_store_id, const std::string& user_key);
  std::string Encode() const;
  int64_t database_id() const { return database_id_; }
  int64_t object_store_id() const { return object_store_id_; }
  const std::string& user_key() const { return user_key_; }

 private:
  int64_t database_id_;
  int64_t object_store_id_;
  std::string user_key_;
};

// IndexDataKey: key for index entries.
class IndexDataKey {
 public:
  IndexDataKey() = default;
  IndexDataKey(int64_t database_id, int64_t object_store_id, int64_t index_id,
               const std::string& user_key, const std::string& primary_key);
  std::string Encode() const;
  int64_t database_id() const { return database_id_; }
  int64_t object_store_id() const { return object_store_id_; }
  int64_t index_id() const { return index_id_; }

 private:
  int64_t database_id_;
  int64_t object_store_id_;
  int64_t index_id_;
  std::string user_key_;
  std::string primary_key_;
};

}  // namespace local_indexed_db

#endif  // LOCAL_INDEXED_DB_KEYSPACE_H_
