#ifndef LOCAL_INDEXED_DB_CURSOR_H_
#define LOCAL_INDEXED_DB_CURSOR_H_

#include "local_indexed_db/storage.h"
#include <memory>
#include <string>
#include <string_view>

namespace local_indexed_db {

// A cursor (range iterator) over stored keys and values.
class Cursor {
 public:
  virtual ~Cursor() = default;
  virtual bool Valid() const = 0;
  virtual void Next() = 0;
  virtual void Prev() = 0;
  virtual std::string_view Key() const = 0;
  virtual std::string_view Value() const = 0;
};

// Factory: create a cursor over a key range [start, end).
std::unique_ptr<Cursor> CreateCursor(IStorage* storage,
                                     std::string_view start,
                                     std::string_view end);

}  // namespace local_indexed_db

#endif  // LOCAL_INDEXED_DB_CURSOR_H_
