#include "local_indexed_db/blob.h"

namespace local_indexed_db {

BlobManager::BlobManager(const std::string& blob_root_path)
    : blob_root_(blob_root_path), next_blob_id_(1) {}
BlobManager::~BlobManager() = default;

int64_t BlobManager::StoreBlob(const std::vector<uint8_t>& data) {
  // Stub: in a real implementation, write to blob_root_ / blob_id and return ID.
  int64_t id = next_blob_id_++;
  return id;
}

bool BlobManager::GetBlob(int64_t blob_id, std::string* path_out) {
  // Stub: in a real implementation, locate blob file.
  *path_out = blob_root_ + "/" + std::to_string(blob_id);
  return true;
}

void BlobManager::MarkForGC(int64_t blob_id) {
  // Stub
}

void BlobManager::GarbageCollect() {
  // Stub
}

}  // namespace local_indexed_db
