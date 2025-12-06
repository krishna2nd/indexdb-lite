#include "local_indexed_db/keyspace.h"
#include "local_indexed_db/encoding.h"

namespace local_indexed_db {

KeyPrefix::KeyPrefix() : type_(INVALID_TYPE), database_id_(0), object_store_id_(0), index_id_(0) {}
KeyPrefix::KeyPrefix(int64_t database_id)
    : type_(DATABASE_METADATA), database_id_(database_id), object_store_id_(0), index_id_(0) {}
KeyPrefix::KeyPrefix(int64_t database_id, int64_t object_store_id)
    : type_(OBJECT_STORE_DATA), database_id_(database_id), object_store_id_(object_store_id), index_id_(0) {}
KeyPrefix::KeyPrefix(int64_t database_id, int64_t object_store_id, int64_t index_id)
    : type_(INDEX_DATA), database_id_(database_id), object_store_id_(object_store_id), index_id_(index_id) {}

std::string KeyPrefix::EncodeEmpty() {
  std::string out;
  EncodeByte(static_cast<unsigned char>(GLOBAL_METADATA), &out);
  return out;
}

std::string KeyPrefix::Encode() const {
  std::string out;
  EncodeByte(static_cast<unsigned char>(type_), &out);
  EncodeInt(database_id_, &out);
  if (type_ >= OBJECT_STORE_DATA) {
    EncodeInt(object_store_id_, &out);
  }
  if (type_ == INDEX_DATA) {
    EncodeInt(index_id_, &out);
  }
  return out;
}

bool KeyPrefix::Decode(const std::string& data, KeyPrefix* result) {
  std::string_view sv(data);
  unsigned char t;
  if (!DecodeByte(&sv, &t)) return false;
  result->type_ = static_cast<Type>(t);
  if (!DecodeInt(&sv, &result->database_id_)) return false;
  if (result->type_ >= OBJECT_STORE_DATA) {
    if (!DecodeInt(&sv, &result->object_store_id_)) return false;
  }
  if (result->type_ == INDEX_DATA) {
    if (!DecodeInt(&sv, &result->index_id_)) return false;
  }
  return true;
}

KeyPrefix::Type KeyPrefix::type() const {
  return type_;
}

ObjectStoreDataKey::ObjectStoreDataKey(int64_t database_id, int64_t object_store_id,
                                       const std::string& user_key)
    : database_id_(database_id), object_store_id_(object_store_id), user_key_(user_key) {}

std::string ObjectStoreDataKey::Encode() const {
  std::string out;
  KeyPrefix prefix(database_id_, object_store_id_);
  out = prefix.Encode();
  out.append(user_key_);
  return out;
}

IndexDataKey::IndexDataKey(int64_t database_id, int64_t object_store_id, int64_t index_id,
                           const std::string& user_key, const std::string& primary_key)
    : database_id_(database_id), object_store_id_(object_store_id), index_id_(index_id),
      user_key_(user_key), primary_key_(primary_key) {}

std::string IndexDataKey::Encode() const {
  std::string out;
  KeyPrefix prefix(database_id_, object_store_id_, index_id_);
  out = prefix.Encode();
  out.append(user_key_);
  out.append(primary_key_);
  return out;
}

}  // namespace local_indexed_db
