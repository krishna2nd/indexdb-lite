cmake_minimum_required(VERSION 3.10)
project(local_indexed_db CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories for all modules.
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/encoding/include
  ${CMAKE_CURRENT_SOURCE_DIR}/storage/include
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include
  ${CMAKE_CURRENT_SOURCE_DIR}/keyspace/include
  ${CMAKE_CURRENT_SOURCE_DIR}/blob/include
  ${CMAKE_CURRENT_SOURCE_DIR}/migration/include
)

# Encoding module sources
set(ENCODING_SOURCES
  encoding/src/encoding.cc
)

# Storage module sources
set(STORAGE_SOURCES
  storage/src/in_memory_storage.cc
)

# Core module sources
set(CORE_SOURCES
  core/src/database.cc
  core/src/transaction.cc
  core/src/cursor.cc
)

# Keyspace module sources
set(KEYSPACE_SOURCES
  keyspace/src/keyspace.cc
)

# Blob module sources
set(BLOB_SOURCES
  blob/src/blob.cc
)

# Migration module sources
set(MIGRATION_SOURCES
  migration/src/migration.cc
)

# Library target: local_indexed_db
add_library(local_indexed_db STATIC
  ${ENCODING_SOURCES}
  ${STORAGE_SOURCES}
  ${CORE_SOURCES}
  ${KEYSPACE_SOURCES}
  ${BLOB_SOURCES}
  ${MIGRATION_SOURCES}
)

target_include_directories(local_indexed_db PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/encoding/include
  ${CMAKE_CURRENT_SOURCE_DIR}/storage/include
  ${CMAKE_CURRENT_SOURCE_DIR}/core/include
  ${CMAKE_CURRENT_SOURCE_DIR}/keyspace/include
  ${CMAKE_CURRENT_SOURCE_DIR}/blob/include
  ${CMAKE_CURRENT_SOURCE_DIR}/migration/include
)

# Executable tests
enable_testing()

add_executable(encoding_test encoding/tests/encoding_test.cpp)
target_link_libraries(encoding_test local_indexed_db)
add_test(NAME EncodingTest COMMAND encoding_test)

add_executable(sortable_key_test encoding/tests/sortable_key_test.cpp)
target_link_libraries(sortable_key_test local_indexed_db)
add_test(NAME SortableKeyTest COMMAND sortable_key_test)

add_executable(database_test core/tests/database_test.cpp)
target_link_libraries(database_test local_indexed_db)
add_test(NAME DatabaseTest COMMAND database_test)

add_executable(keyspace_test keyspace/tests/keyspace_test.cpp)
target_link_libraries(keyspace_test local_indexed_db)
add_test(NAME KeyspaceTest COMMAND keyspace_test)

add_executable(integration_test core/tests/integration_test.cpp)
target_link_libraries(integration_test local_indexed_db)
add_test(NAME IntegrationTest COMMAND integration_test)

# Example executable
add_executable(example examples/example.cpp)
target_link_libraries(example local_indexed_db)

# Export package config
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  local_indexed_dbConfigVersion.cmake
  VERSION 1.0.0
  COMPATIBILITY AnyNewerVersion
)

install(TARGETS local_indexed_db
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(DIRECTORY
  encoding/include/local_indexed_db
  storage/include/local_indexed_db
  core/include/local_indexed_db
  keyspace/include/local_indexed_db
  blob/include/local_indexed_db
  migration/include/local_indexed_db
  DESTINATION include
)
