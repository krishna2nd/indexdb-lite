# Local IndexedDB Node.js Module - Setup & Installation Guide

## Overview

This is a Node.js native addon that exposes the portable IndexedDB library as a JavaScript module. It provides a synchronous API for database operations with full support for IndexedDB key types and batch transactions.

## Prerequisites

### System Requirements
- **Node.js**: 14.x or higher (tested with 14.x, 16.x, 18.x)
- **npm**: 6.x or higher
- **Python**: 3.6+ (required by node-gyp for compilation)
- **Build Tools**:
  - **macOS**: Xcode Command Line Tools (`xcode-select --install`)
  - **Linux**: `build-essential`, `python3`, `git`
  - **Windows**: Visual Studio Build Tools 2015 or later

### Ubuntu/Debian Installation
```bash
sudo apt-get update
sudo apt-get install -y build-essential python3 git
```

### macOS Installation
```bash
xcode-select --install
brew install python3
```

## Installation Steps

### 1. Install Dependencies

Navigate to the nodejs module directory and install npm dependencies:

```bash
cd /path/to/nodejs
npm install
```

This installs:
- **nan**: 2.18.0 (Node.js native addon framework)
- **mocha**: 10.2.0 (test runner)
- **chai**: 4.3.10 (assertion library)
- **node-gyp**: 9.4.0 (build tool for native addons)

### 2. Build the Native Module

```bash
npm run build
```

This command:
1. Invokes node-gyp configure (detects Node.js version, build tools)
2. Compiles C++ sources (binding.cc + portable library files)
3. Links the native module
4. Places output in `build/Release/local_indexed_db_native.node`

**Expected output:**
```
gyp info it worked if it ends with ok
gyp info using node-gyp@9.4.0
gyp info using node@16.x.x | win32 | x64
...
gyp info ok
```

### 3. Verify Installation

Test that the module loads correctly:

```bash
node -e "const db = require('./lib/index.js'); console.log('Module loaded:', typeof db.Database);"
```

Expected output:
```
Module loaded: function
```

## Quick Start

### Basic Usage

```javascript
const { Database, IDBKey, Encoding, Status } = require('./lib/index.js');

// Create a new database instance
const db = new Database();

// Put a key-value pair
let status = db.put('user:1', JSON.stringify({ name: 'Alice', age: 30 }));
console.log('Put status:', Status.isOk(status) ? 'OK' : 'FAILED');

// Get a value
const result = db.get('user:1');
console.log('Retrieved:', result);

// Delete a key
db.delete('user:1');

// Check if key exists (after deletion)
const notFound = db.get('user:1');
console.log('After delete:', Status.isNotFound(notFound) ? 'NOT_FOUND' : 'ERROR');

// Batch operations (atomic)
const batch = new db.WriteBatch();
batch.put('user:2', JSON.stringify({ name: 'Bob', age: 25 }));
batch.put('user:3', JSON.stringify({ name: 'Carol', age: 28 }));
batch.delete('user:1');

status = db.commitBatch(batch);
console.log('Batch commit:', Status.isOk(status) ? 'OK' : 'FAILED');
```

### Running Tests

```bash
npm test
```

This runs the Mocha test suite (`test/database.test.js`), which covers:
- Basic put/get/delete operations
- Key-value round-trips
- Batch transactions
- Error handling (NOT_FOUND states)
- IDBKey type detection and factories
- Key encoding/decoding utilities

Expected output:
```
Database API
  ✓ should put and get a value
  ✓ should return NOT_FOUND for missing keys
  ✓ should overwrite existing keys
  ...
  8 passing (Xs)
```

### Running the Example

```bash
node example.js
```

This demonstrates:
- Creating multiple object stores (users, settings)
- Putting and retrieving objects
- Deleting records
- Working with different IndexedDB key types (null, number, string, date, array)
- Key encoding/decoding round-trips

## API Reference

### Database Class

#### Constructor
```javascript
const db = new Database();
```
Creates a new in-memory database instance.

#### Methods

**put(key: string, value: string): number**
```javascript
const status = db.put('myKey', 'myValue');
if (Status.isOk(status)) {
  console.log('Insert/update successful');
}
```
Inserts or updates a key-value pair. Returns a Status code.

**get(key: string): string | null**
```javascript
const value = db.get('myKey');
if (Status.isNotFound(value)) {
  console.log('Key not found');
} else {
  console.log('Value:', value);
}
```
Retrieves a value by key. Returns value string or Status code (NOT_FOUND/ERROR).

**delete(key: string): number**
```javascript
const status = db.delete('myKey');
if (Status.isOk(status)) {
  console.log('Deletion successful');
}
```
Deletes a key from the database. Returns a Status code.

**commitBatch(batch: WriteBatch): number**
```javascript
const batch = new db.WriteBatch();
batch.put('key1', 'value1');
batch.put('key2', 'value2');
batch.delete('key3');
const status = db.commitBatch(batch);
```
Atomically commits all operations in a batch. Returns a Status code.

### WriteBatch Class

**put(key: string, value: string): void**
Adds a put operation to the batch.

**delete(key: string): void**
Adds a delete operation to the batch.

### IDBKey Class

Factory methods for creating IndexedDB key types:

```javascript
// Null key
const nullKey = IDBKey.null();

// Numeric key
const numKey = IDBKey.number(42);

// String key
const strKey = IDBKey.string('myKey');

// Date key (milliseconds since epoch)
const dateKey = IDBKey.date(Date.now());

// Array key (mixed types)
const arrayKey = IDBKey.array([
  IDBKey.string('user'),
  IDBKey.number(123),
  IDBKey.date(Date.now())
]);

// Get type
console.log(numKey.getType()); // 'Number'
```

### Encoding Class

**encodeKey(key: any): string**
Encodes an IndexedDB key to a sortable string representation.

**decodeKey(encoded: string): any**
Decodes a previously encoded key back to its original type.

```javascript
const key = { type: 'Date', value: 1672531200000 };
const encoded = Encoding.encodeKey(key);
const decoded = Encoding.decodeKey(encoded);
console.log(decoded); // Reconstructed key
```

### Status Constants

```javascript
const Status = {
  OK: 0,           // Operation succeeded
  NOT_FOUND: 1,    // Key does not exist
  ERROR: 2         // Operation failed
};
```

**Helper Methods:**

```javascript
Status.isOk(status)         // true if status === 0
Status.isNotFound(status)   // true if status === 1
Status.isError(status)      // true if status === 2
```

## Building from Source

### Directory Structure

```
nodejs/
├── binding.gyp              # node-gyp configuration
├── package.json             # npm metadata
├── lib/
│   └── index.js            # JavaScript wrapper API
├── src/
│   ├── binding.cc          # NaN C++ bindings
│   ├── database_wrap.cc    # Database wrapper stub
│   └── transaction_wrap.cc # Transaction wrapper stub
├── test/
│   └── database.test.js    # Mocha test suite
├── example.js              # Usage example
├── README.md               # API documentation
├── SETUP.md                # This file
└── .gitignore              # Git exclusions
```

### Build Configuration (binding.gyp)

The `binding.gyp` file configures:
- **targets**: local_indexed_db_native (native module)
- **sources**: 11 C++ source files (binding.cc + 10 portable library files)
- **include_dirs**: Portable library headers
- **cflags_cc**: C++17 standard flag
- **libraries**: Platform-specific linking (pthread on Linux)

### Advanced Build Options

**Clean build:**
```bash
rm -rf build
npm run build
```

**Debug build (with symbols):**
```bash
npm run build -- --debug
```

**Verbose output:**
```bash
npm run build -- --verbose
```

## Troubleshooting

### Build Failures

#### Error: "python not found"
```
Solution: Install Python 3
Ubuntu/Debian: sudo apt-get install python3
macOS: brew install python3
Windows: Download from python.org
```

#### Error: "node-gyp configure" fails
```
Solution: Ensure build tools are installed
Ubuntu/Debian: sudo apt-get install build-essential
macOS: xcode-select --install
Windows: Install Visual Studio Build Tools
```

#### Error: "binding not found"
```
Solution: Rebuild the module
rm -rf build/
npm run build
```

### Runtime Issues

#### Module loads but Database is undefined
```javascript
// ❌ Incorrect
const { Database } = require('./lib/index.js');
const db = Database(); // Error!

// ✅ Correct
const { Database } = require('./lib/index.js');
const db = new Database();
```

#### Operations return ERROR status
```
Solution: Check key/value encoding
- Keys and values must be strings
- Use JSON.stringify() for objects
- UTF-8 encoding is assumed
```

#### Tests fail to load module
```
Solution: Ensure native module is built
npm run build
npm test
```

## Performance Notes

- **Synchronous operations**: All database operations are synchronous (block until complete)
- **In-memory storage**: Default backend stores all data in memory (no persistence)
- **Batch efficiency**: Use batch operations for multi-key updates
- **String encoding**: All keys and values are UTF-8 strings

### Benchmark (Expected Performance)
- **Put operation**: < 1ms (in-memory)
- **Get operation**: < 0.5ms (in-memory)
- **Delete operation**: < 0.5ms (in-memory)
- **Batch commit (10 ops)**: < 5ms (in-memory)

## Next Steps

1. **Run tests**: `npm test`
2. **Run example**: `node example.js`
3. **Explore API**: Read `README.md` for detailed API documentation
4. **Integrate**: Import `lib/index.js` in your project

## Additional Resources

- **Portable Library**: See `/portable` directory for C++ library documentation
- **IndexedDB Spec**: https://w3c.github.io/IndexedDB/
- **Node.js Native Addons**: https://nodejs.org/api/addons.html
- **NaN Documentation**: https://github.com/nodejs/nan

## Support & Contributing

For issues, feature requests, or contributions:
1. Check existing tests (`test/database.test.js`)
2. Review example usage (`example.js`)
3. Consult API documentation (`README.md`)
4. Examine native bindings (`src/binding.cc`)

## License

See parent directory LICENSE file for terms.
