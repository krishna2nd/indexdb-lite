#ifndef LOCAL_INDEXED_DB_H_
#define LOCAL_INDEXED_DB_H_

// Master header for the Local Indexed DB portable library.
// Includes all public APIs for encoding, storage, transactions, and keyspace management.

#include "local_indexed_db/encoding.h"
#include "local_indexed_db/storage.h"
#include "local_indexed_db/database.h"
#include "local_indexed_db/transaction.h"
#include "local_indexed_db/cursor.h"
#include "local_indexed_db/keyspace.h"
#include "local_indexed_db/blob.h"
#include "local_indexed_db/migration.h"

namespace local_indexed_db {

// Convenience namespace for common operations.

// Create a new in-memory database for testing.
inline std::unique_ptr<Database> CreateInMemoryDatabase() {
  return std::make_unique<Database>(CreateInMemoryStorage());
}

// Create a new transaction.
inline std::unique_ptr<Transaction> CreateTransaction(std::unique_ptr<IStorage> storage) {
  return std::make_unique<Transaction>(std::move(storage));
}

}  // namespace local_indexed_db

#endif  // LOCAL_INDEXED_DB_H_
