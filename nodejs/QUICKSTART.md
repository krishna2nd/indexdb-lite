# Quick Start Guide - 5 Minutes to Running IndexedDB in Node.js

## Installation (2 minutes)

```bash
# Navigate to nodejs module
cd /path/to/nodejs

# Install dependencies
npm install

# Build native module
npm run build
```

**Expected result:** No errors, binary created in `build/Release/`

## Test It Works (1 minute)

```bash
# Run test suite
npm test

# Expected: 8 passing
```

## First Program (2 minutes)

Create `test_my_db.js`:

```javascript
const { Database, Status } = require('./lib/index.js');

// Create database
const db = new Database();

// Basic operations
db.put('hello', 'world');
console.log('Get hello:', db.get('hello'));

// Check what works
console.log('Put OK:', Status.isOk(0));
console.log('Status NOT_FOUND:', Status.NOT_FOUND);

// Batch insert
const batch = new db.WriteBatch();
batch.put('key1', 'value1');
batch.put('key2', 'value2');
db.commitBatch(batch);

// Verify batch
console.log('After batch:');
console.log('  key1:', db.get('key1'));
console.log('  key2:', db.get('key2'));
```

Run it:
```bash
node test_my_db.js
```

**Output:**
```
Get hello: world
Put OK: true
Status NOT_FOUND: 1
After batch:
  key1: value1
  key2: value2
```

## Key API Cheat Sheet

```javascript
const { Database, IDBKey, Encoding, Status } = require('./lib/index.js');

const db = new Database();

// Put/Get/Delete
db.put('key', 'value');           // Returns: 0 (OK) or 2 (ERROR)
db.get('key');                     // Returns: value string or 1 (NOT_FOUND)
db.delete('key');                  // Returns: 0 (OK)

// Batch operations
const batch = new db.WriteBatch();
batch.put('k1', 'v1');
batch.delete('k2');
db.commitBatch(batch);             // Atomic

// Check status
Status.isOk(0);                    // true
Status.isNotFound(1);              // true
Status.isError(2);                 // true

// IDBKey factories
IDBKey.null();                     // Null key
IDBKey.number(42);                 // Number
IDBKey.string('text');             // String
IDBKey.date(Date.now());           // Date/timestamp
IDBKey.array([...]);               // Mixed array

// Encoding
const encoded = Encoding.encodeKey(someKey);
const decoded = Encoding.decodeKey(encoded);
```

## Common Patterns

### User Store
```javascript
const db = new Database();

// Save user
db.put('user:1', JSON.stringify({ 
  id: 1, 
  name: 'Alice',
  email: 'alice@example.com'
}));

// Retrieve user
const userJson = db.get('user:1');
const user = JSON.parse(userJson);
console.log(user.name); // Alice
```

### Multi-Record Batch
```javascript
const batch = new db.WriteBatch();

batch.put('user:1', JSON.stringify({ name: 'Alice' }));
batch.put('user:2', JSON.stringify({ name: 'Bob' }));
batch.put('user:3', JSON.stringify({ name: 'Carol' }));
batch.delete('user:999');  // Remove obsolete

const status = db.commitBatch(batch);
if (Status.isOk(status)) {
  console.log('Batch saved atomically');
}
```

### Key Existence Check
```javascript
const value = db.get('someKey');

if (Status.isNotFound(value)) {
  console.log('Key not found');
} else if (Status.isError(value)) {
  console.log('Error retrieving key');
} else {
  console.log('Found value:', value);
}
```

## Troubleshooting

**Problem: "Cannot find module"**
```bash
# Ensure build succeeded
npm run build
# Check output is created
ls -la build/Release/
```

**Problem: Tests fail**
```bash
# Clean rebuild
rm -rf build node_modules
npm install
npm run build
npm test
```

**Problem: Operations return ERROR (status 2)**
- Ensure keys and values are strings
- Use JSON.stringify() for objects
- Check batch operations are properly committed

## What's Next?

- Read `README.md` for complete API documentation
- Run `node example.js` for feature showcase
- Check `test/database.test.js` for usage examples
- See `SETUP.md` for advanced configuration

## Performance Expectations

- **Get/Put**: < 1ms per operation (in-memory)
- **Batch (10 ops)**: < 5ms
- **No persistence**: Data lost on exit (use file backend for durability)

---

**Questions?** See README.md or check test examples in `test/database.test.js`
