#ifndef LOCAL_INDEXED_DB_MIGRATION_H_
#define LOCAL_INDEXED_DB_MIGRATION_H_

#include <cstdint>
#include <string>

namespace local_indexed_db {

// Current and supported schema versions.
constexpr int64_t kLatestSchemaVersion = 1;
constexpr int64_t kEarliestSupportedSchemaVersion = 1;

// Migration functions to upgrade a store from old to new schema.
// Returns true on success, false on failure.
bool RunMigration(int64_t from_version, int64_t to_version, const std::string& db_path);

}  // namespace local_indexed_db

#endif  // LOCAL_INDEXED_DB_MIGRATION_H_
