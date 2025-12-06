// Simple example demonstrating local-indexed-db Node.js binding

const { Database, IDBKey, Encoding, Status } = require('./lib/index');

console.log('=== Local Indexed DB Node.js Example ===\n');

// Create database
const db = new Database();
console.log('1. Created in-memory database\n');

// Store some data
console.log('2. Storing key-value pairs...');
db.put('user:1', JSON.stringify({ id: 1, name: 'Alice', role: 'admin' }));
db.put('user:2', JSON.stringify({ id: 2, name: 'Bob', role: 'user' }));
db.put('user:3', JSON.stringify({ id: 3, name: 'Charlie', role: 'user' }));
console.log('   Stored 3 users\n');

// Retrieve data
console.log('3. Retrieving data...');
const result = db.get('user:1');
if (Database.isOk(result)) {
  const user = JSON.parse(result.value);
  console.log(`   Found user: ${user.name} (${user.role})\n`);
}

// List all users
console.log('4. Listing all users...');
for (let i = 1; i <= 3; i++) {
  const r = db.get(`user:${i}`);
  if (Database.isOk(r)) {
    const u = JSON.parse(r.value);
    console.log(`   user:${i} -> ${u.name}`);
  }
}
console.log('');

// Batch operations
console.log('5. Batch operations...');
const batch = [
  ['settings:theme', 'dark'],
  ['settings:language', 'en'],
  ['settings:notifications', 'true'],
  ['temp:cache', null]  // delete if exists
];
const batchStatus = db.commitBatch(batch);
console.log(`   Batch commit status: ${batchStatus === Status.OK ? 'OK' : 'ERROR'}`);
console.log(`   Retrieved theme: ${db.get('settings:theme').value}\n`);

// IDBKey examples
console.log('6. IDBKey examples...');
const keyTypes = [
  ['Number', new IDBKey(42.5)],
  ['String', new IDBKey('hello')],
  ['Null', IDBKey.null()],
  ['Date', IDBKey.date(1609459200000)],
  ['Array', new IDBKey([1, 'two', null])]
];

for (const [name, key] of keyTypes) {
  console.log(`   ${name}: type=${key.type}, value=${JSON.stringify(key.value)}`);
}
console.log('');

// Encoding
console.log('7. Key encoding/decoding...');
const testKey = new IDBKey('encoded_test');
const encoded = Encoding.encodeKey(testKey);
const decoded = Encoding.decodeKey(encoded);
console.log(`   Original: ${testKey.value}`);
console.log(`   Encoded: ${encoded.toString('base64').slice(0, 32)}...`);
console.log(`   Decoded: ${decoded.value}`);
console.log(`   Match: ${decoded.value === testKey.value ? 'YES' : 'NO'}\n`);

// Store encoded keys
console.log('8. Storing with encoded keys...');
const key1 = Encoding.encodeKey(new IDBKey('product:laptop'));
const key2 = Encoding.encodeKey(new IDBKey('product:monitor'));
db.put(key1.toString('base64'), JSON.stringify({ name: 'Laptop', price: 999 }));
db.put(key2.toString('base64'), JSON.stringify({ name: 'Monitor', price: 299 }));
console.log('   Stored encoded product keys\n');

console.log('=== Example Complete ===\n');
