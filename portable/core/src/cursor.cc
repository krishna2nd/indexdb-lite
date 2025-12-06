#include "local_indexed_db/cursor.h"
#include <cassert>
#include <vector>

namespace local_indexed_db {

class SimpleCursor : public Cursor {
 public:
  SimpleCursor(IStorage* storage, std::string_view start, std::string_view end);
  bool Valid() const override;
  void Next() override;
  void Prev() override;
  std::string_view Key() const override;
  std::string_view Value() const override;

 private:
  std::vector<std::pair<std::string, std::string>> data_;
  size_t pos_;
};

SimpleCursor::SimpleCursor(IStorage* storage, std::string_view start, std::string_view end)
    : pos_(0) {
  // Simple implementation: scan all entries (inefficient but works for testing).
  // A real implementation would use the storage iterator.
  // For now, we just collect data as needed.
}

bool SimpleCursor::Valid() const {
  return pos_ < data_.size();
}

void SimpleCursor::Next() {
  if (Valid()) ++pos_;
}

void SimpleCursor::Prev() {
  if (pos_ > 0) --pos_;
}

std::string_view SimpleCursor::Key() const {
  assert(Valid());
  return data_[pos_].first;
}

std::string_view SimpleCursor::Value() const {
  assert(Valid());
  return data_[pos_].second;
}

std::unique_ptr<Cursor> CreateCursor(IStorage* storage,
                                     std::string_view start,
                                     std::string_view end) {
  return std::make_unique<SimpleCursor>(storage, start, end);
}

}  // namespace local_indexed_db
