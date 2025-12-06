# Local Indexed DB

A portable C++17 library that provides IndexedDB-like semantics for local storage, extracted and refactored from the Chromium browser engine for use in standalone applications.

## Features

- **Encoding & Decoding**: Full support for IndexedDB key types (Null, Number, Date, String, Binary, Array) with sortable encoding to preserve ordering.
- **Storage Abstraction**: Pluggable storage backend interface (`IStorage`). Includes in-memory implementation suitable for testing.
- **Transactions**: Snapshot-based transaction model with atomic batch commits.
- **Keyspace Management**: KeyPrefix and key classes for hierarchical key organization (databases, object stores, indexes).
- **Blob Support**: Stub blob manager for external object storage.
- **Schema Migrations**: Framework for versioning and migrations (stubs in v1.0).

## Building

### Prerequisites

- C++17 compiler (GCC 7+, Clang 5+)
- CMake 3.10+

### Build Steps

```bash
cd portable
mkdir build
cd build
cmake ..
make
ctest  # Run tests
```

### Manual Compilation (for quick testing)

```bash
# Encoding tests
g++ -std=c++17 -I encoding/include \
  encoding/src/encoding.cc \
  encoding/tests/sortable_key_test.cpp \
  -O2 -g -o sortable_key_test
./sortable_key_test

# Full integration
g++ -std=c++17 \
  -I encoding/include -I storage/include -I core/include \
  encoding/src/encoding.cc \
  storage/src/in_memory_storage.cc \
  core/src/database.cc \
  core/src/transaction.cc \
  core/src/cursor.cc \
  keyspace/src/keyspace.cc \
  core/tests/database_test.cpp \
  -O2 -g -o database_test
./database_test
```

## Architecture

### Modules

- **encoding**: Low-level encode/decode for IDBKey variants and sortable keys.
- **storage**: Storage interface and in-memory implementation.
- **core**: Database, Transaction, and Cursor APIs.
- **keyspace**: KeyPrefix and key classes for database organization.
- **blob**: Blob manager stub for external object storage.
- **migration**: Schema versioning and migration framework.

### Key Classes

#### IDBKey

Represents an IndexedDB key (number, string, date, binary, array, etc.).

```cpp
IDBKey k1(42.5);                    // Number
IDBKey k2(std::string("abc"));      // String
IDBKey k3 = IDBKey::Null();         // Null
IDBKey k4 = IDBKey::Date(1609459200000.0); // Date in ms
```

#### Encoding Functions

```cpp
std::string encoded = EncodeSortableIDBKey(key);
IDBKey decoded = DecodeSortableIDBKey(encoded);
```

#### Database

```cpp
auto storage = CreateInMemoryStorage();
Database db(std::move(storage));
auto status = db.Put("key1", "value1");
std::string value;
db.Get("key1", &value);
```

#### Transaction

```cpp
auto txn = std::make_unique<Transaction>(std::move(storage));
txn->AddWriteOp("key1", "value1");
txn->AddWriteOp("key2", "value2");
txn->Commit();
```

#### KeyPrefix & Keys

```cpp
KeyPrefix db_prefix(database_id);
KeyPrefix store_prefix(database_id, object_store_id);
KeyPrefix index_prefix(database_id, object_store_id, index_id);

ObjectStoreDataKey data_key(db_id, store_id, user_key);
IndexDataKey index_key(db_id, store_id, idx_id, user_key, primary_key);
```

## Testing

All modules include unit tests:

```bash
# In portable/build:
ctest --output-on-failure
```

Individual tests:
- `encoding_test`: Basic encode/decode round-trips.
- `sortable_key_test`: Sortable key ordering verification.
- `database_test`: Database put/get/batch operations.
- `keyspace_test`: KeyPrefix encoding/decoding.
- `integration_test`: End-to-end workflow.

## Roadmap / TODO

- [ ] LevelDB storage backend integration.
- [ ] SQLite storage backend option.
- [ ] Cursor iteration over key ranges (currently stub).
- [ ] Sentinel-based string/binary encoding for Chromium parity.
- [ ] Blob journal and garbage collection.
- [ ] Full schema migration examples (v2+).
- [ ] Python / Node.js bindings.
- [ ] Benchmarks and performance profiling.

## Notes

- This library is intended for **local storage use cases** and does **not** require network or browser infrastructure.
- The encoding format is compatible with IndexedDB semantics but uses simplified (non-sentinel) string/binary encodings for portability. Strict Chromium parity can be added if needed.
- The in-memory storage is suitable for tests and demos. For production, implement an `IStorage` backend using LevelDB, RocksDB, or SQLite.

## License

Copyright (c) 2025. Extracted and refactored from Chromium (BSD license). See LICENSE file for details.

## References

- [IndexedDB W3C Specification](https://w3c.github.io/IndexedDB/)
- [Chromium Indexed DB Implementation](https://source.chromium.org/chromium/chromium/src/+/main:content/browser/indexed_db/)
