# local-indexed-db (Node.js Binding)

> ✅ **Published to npm as `indexdb-lite`** - [View on npm](https://www.npmjs.com/package/indexdb-lite)

A fast, efficient Node.js binding for the Local Indexed DB C++ library. Store and retrieve data with IndexedDB semantics using native performance.

## Installation

### From npm (recommended)

```bash
npm install indexdb-lite
```

### From source

```bash
git clone https://github.com/krishna2nd/indexdb-lite.git
cd nodejs
npm install
npm run build
```

### Requirements

- Node.js 12.0.0 or higher
- Python 3.6+ (for node-gyp)
- C++17 compatible compiler (GCC 7+, Clang 5+)

## Quick Start

```javascript
const { Database, IDBKey, Encoding, Status } = require('indexdb-lite');

// Create a database
const db = new Database();

// Store values
db.put('key1', 'value1');
db.put('user:123', JSON.stringify({ name: 'Alice', age: 30 }));

// Retrieve values
const result = db.get('key1');
if (Database.isOk(result)) {
  console.log('Found:', result.value);
} else if (Database.isNotFound(result)) {
  console.log('Key not found');
}

// Atomic batch operations
const batch = [
  ['key1', 'new_value'],
  ['key2', 'another_value'],
  ['key3', null]  // null = delete
];
db.commitBatch(batch);

// Delete a key
db.delete('key1');
```

## API

### Database

#### `new Database()`

Create a new in-memory database instance.

```javascript
const db = new Database();
```

#### `put(key, value) -> number`

Store a key-value pair. Returns status code (0 = OK).

```javascript
const status = db.put('mykey', 'myvalue');
```

- [Local Indexed DB (C++ Library)](../portable/)
````markdown
# local-indexed-db (Node.js Binding)

> ✅ **Published to npm as `indexdb-lite`** - [View on npm](https://www.npmjs.com/package/indexdb-lite)

A fast, efficient Node.js binding for the Local Indexed DB C++ library. The npm package includes a small JavaScript wrapper plus a `portable/` folder that contains headers and prebuilt portable artifacts (when available) so downstream native integrations can link or consume headers without requiring a full source build.

## Installation

### From npm (recommended)

```bash
npm install indexdb-lite@1.0.2
```

The package ships with a `portable/` directory that contains the C++ headers and (when present) prebuilt static libraries under `portable/build/`. This makes it convenient for consumers who need the library headers or wish to build native components that link against the portable artifacts.

### From source

```bash
git clone https://github.com/krishna2nd/indexdb-lite.git
cd nodejs
npm install
# If you want to build the native binding from sources (optional)
npm run build
```

### Requirements (only for building from source)

- Node.js 14.0.0 or higher
- Python 3.6+ (for node-gyp)
- C++17 compatible compiler (GCC 7+, Clang 5+)

## Quick Start (Node.js)

```javascript
const { Database, IDBKey, Encoding, Status } = require('indexdb-lite');

// Create a database instance
const db = new Database();

// Store values
db.put('key1', 'value1');

// Retrieve values
const result = db.get('key1');
if (Database.isOk(result)) {
  console.log('Found:', result.value);
}
```

If you prefer a complete runnable example, see `nodejs/examples/example-company-db-npm.js`. That script demonstrates storing departments and employees in a single on-disk database file, then performing correlation queries (employee -> department and department -> employees). The example will use the published native binding if available, or a small JS fallback when the native addon is not present.

## Included Files (high-level)

- `lib/` - JS wrapper that talks to the native addon
- `index.d.ts` - Basic TypeScript type hints
- `portable/` - C++ headers and prebuilt portable artifacts (headers under `portable/include/`, static libs under `portable/build/` when present)
- `examples/` - Usage examples including `example-company-db-npm.js`

## Examples

Run the example that demonstrates employees <> departments correlations:

```bash
node examples/example-company-db-npm.js
```

This will create a small DB file at `nodejs/data/npm_company.db` when run with the fallback JS store. When the native binding is present, behavior will follow the native implementation.

## Testing

```bash
npm test
```

## Building (optional)

```bash
npm run build     # Build native module (if you need the native binding)
npm run clean     # Clean build artifacts
```

## Notes

- The npm package includes prebuilt portable artifacts to simplify integration. If you need to build against the C++ sources directly, check `portable/` and the examples in `portable/`.
- If you plan to publish a downstream native module that links to the portable library, use the headers under `portable/include/` and the static library under `portable/build/`.

## License

BSD-3-Clause

## References

- [Local Indexed DB (C++ Library)](../portable/)
- [IndexedDB W3C Spec](https://w3c.github.io/IndexedDB/)

````
Utilities for encoding/decoding keys.

