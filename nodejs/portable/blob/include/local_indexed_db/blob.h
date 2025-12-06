#ifndef LOCAL_INDEXED_DB_BLOB_H_
#define LOCAL_INDEXED_DB_BLOB_H_

#include <cstdint>
#include <string>
#include <vector>

namespace local_indexed_db {

// Blob entry: represents a blob stored externally (typically on-disk).
struct BlobEntry {
  int64_t blob_id;
  int64_t size;
  std::string path;  // File path or reference
};

// BlobManager: manages blob storage, typically to a file root.
class BlobManager {
 public:
  explicit BlobManager(const std::string& blob_root_path = "");
  ~BlobManager();

  // Store a blob and return its ID.
  int64_t StoreBlob(const std::vector<uint8_t>& data);

  // Retrieve blob path or contents.
  bool GetBlob(int64_t blob_id, std::string* path_out);

  // Mark a blob for garbage collection (stub for now).
  void MarkForGC(int64_t blob_id);

  // Garbage collect unreferenced blobs (stub for now).
  void GarbageCollect();

 private:
  std::string blob_root_;
  int64_t next_blob_id_;
};

}  // namespace local_indexed_db

#endif  // LOCAL_INDEXED_DB_BLOB_H_
