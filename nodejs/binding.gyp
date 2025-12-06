{
  "targets": [
    {
      "target_name": "local_indexed_db_native",
      "sources": [
        "src/binding.cc",
        "src/database_wrap.cc",
        "src/transaction_wrap.cc",
        "portable/encoding/src/encoding.cc",
        "portable/storage/src/in_memory_storage.cc",
        "portable/core/src/database.cc",
        "portable/core/src/transaction.cc",
        "portable/core/src/cursor.cc",
        "portable/keyspace/src/keyspace.cc",
        "portable/blob/src/blob.cc",
        "portable/migration/src/migration.cc"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "portable/encoding/include",
        "portable/storage/include",
        "portable/core/include",
        "portable/keyspace/include",
        "portable/blob/include",
        "portable/migration/include"
      ],
      "cflags": [
        "-std=c++17",
        "-Wall",
        "-O2"
      ],
      "cflags_cc": [
        "-std=c++17",
        "-Wall",
        "-O2"
      ]
    }
  ]
}
