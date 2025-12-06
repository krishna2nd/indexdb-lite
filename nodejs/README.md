# local-indexed-db (Node.js Binding)

A fast, efficient Node.js binding for the Local Indexed DB C++ library. Store and retrieve data with IndexedDB semantics using native performance.

## Installation

### From source

```bash
npm install local-indexed-db
# or
git clone <repo>
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
const { Database, IDBKey, Encoding, Status } = require('local-indexed-db');

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

#### `get(key) -> Object`

Retrieve a value by key. Returns `{status, value}`.

```javascript
const result = db.get('mykey');
if (Database.isOk(result)) {
  console.log(result.value);
}
```

Status codes:
- `Status.OK` (0): Success
- `Status.NOT_FOUND` (1): Key doesn't exist
- `Status.ERROR` (2): Error occurred

#### `delete(key) -> number`

Delete a key-value pair. Returns status code.

```javascript
db.delete('mykey');
```

#### `commitBatch(batch) -> number`

Commit multiple operations atomically.

```javascript
const batch = [
  ['key1', 'value1'],
  ['key2', 'value2'],
  ['key3', null]  // delete
];
db.commitBatch(batch);
```

#### `Database.isOk(result) -> boolean`

Check if a get() result is successful.

```javascript
if (Database.isOk(db.get('key'))) { /* ... */ }
```

#### `Database.isNotFound(result) -> boolean`

Check if a get() result indicates key not found.

```javascript
if (Database.isNotFound(db.get('key'))) { /* ... */ }
```

### IDBKey

Utilities for working with IndexedDB key types.

#### `new IDBKey(value)`

Create a key from a value. Automatically detects type (number, string, array, etc.).

```javascript
const numKey = new IDBKey(42);
const strKey = new IDBKey('abc');
const arrKey = new IDBKey([1, 'two', null]);
```

#### `IDBKey.null()`

Create a null key.

```javascript
const nullKey = IDBKey.null();
```

#### `IDBKey.date(ms)`

Create a date key (stored as milliseconds since epoch).

```javascript
const dateKey = IDBKey.date(Date.now());
```

#### Key Types

```javascript
IDBKey.TYPES = {
  INVALID: 0,
  NULL: 1,
  NUMBER: 2,
  DATE: 3,
  STRING: 4,
  BINARY: 5,
  ARRAY: 6
};
```

### Encoding

Utilities for encoding/decoding keys.

#### `Encoding.encodeKey(key) -> Buffer`

Encode an IDBKey to a buffer.

```javascript
const key = new IDBKey('test');
const encoded = Encoding.encodeKey(key);
```

#### `Encoding.decodeKey(buffer) -> IDBKey`

Decode a buffer back to an IDBKey.

```javascript
const decoded = Encoding.decodeKey(encoded);
```

## Examples

### JSON Documents

```javascript
const db = new Database();

const user = { id: 1, name: 'Alice', email: 'alice@example.com' };
db.put('user:1', JSON.stringify(user));

const result = db.get('user:1');
const userData = JSON.parse(result.value);
console.log(userData.name);
```

### Numeric Keys

```javascript
const db = new Database();

// Store with numeric keys
const scores = [
  ['score:100', 'player1'],
  ['score:95', 'player2'],
  ['score:88', 'player3']
];

db.commitBatch(scores);

// Retrieve
const result = db.get('score:100');
console.log(result.value);  // 'player1'
```

### Key Prefixes

```javascript
const db = new Database();

// Use prefixes for hierarchical storage
db.put('users:123:profile', '...');
db.put('users:123:settings', '...');
db.put('users:456:profile', '...');

// Retrieve user data
const profile = db.get('users:123:profile');
const settings = db.get('users:123:settings');
```

### Transactions with Error Handling

```javascript
const db = new Database();

function transferFunds(from, to, amount) {
  const fromResult = db.get(`account:${from}:balance`);
  const toResult = db.get(`account:${to}:balance`);

  if (!Database.isOk(fromResult) || !Database.isOk(toResult)) {
    console.error('Account not found');
    return false;
  }

  const fromBalance = parseFloat(fromResult.value);
  const toBalance = parseFloat(toResult.value);

  if (fromBalance < amount) {
    console.error('Insufficient funds');
    return false;
  }

  const batch = [
    [`account:${from}:balance`, String(fromBalance - amount)],
    [`account:${to}:balance`, String(toBalance + amount)]
  ];

  return db.commitBatch(batch) === Status.OK;
}
```

## Testing

```bash
npm test
```

Run tests with verbose output:

```bash
npm test -- --reporter spec
```

## Building

```bash
npm run build     # Build native module
npm run clean     # Clean build artifacts
```

## Performance Notes

- All operations are synchronous (blocking).
- The native C++ backend provides fast key-value operations.
- Batch commits are atomic but single-threaded.
- In-memory storage is suitable for testing and caching; for persistent storage, implement a file-backed `IStorage` backend in C++.

## Future Enhancements

- [ ] Async/Promise API
- [ ] Persistent file-backed storage
- [ ] LevelDB backend option
- [ ] SQLite backend option
- [ ] Cursor/range iteration support
- [ ] Transaction API
- [ ] Blob support

## License

BSD-3-Clause (compatible with Chromium source)

## References

- [Local Indexed DB (C++ Library)](../portable/)
- [IndexedDB W3C Spec](https://w3c.github.io/IndexedDB/)
