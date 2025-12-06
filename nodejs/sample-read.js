const FileDB = require('./file_db');

const db = new FileDB('sample-index.db');

console.log('Reading data from sample-index.db...');

const keys = db.keys();
console.log('Keys in DB:', keys);

for (const k of keys) {
  const v = db.get(k);
  // Try to pretty-print JSON values
  let parsed = v;
  try {
    parsed = JSON.parse(v);
  } catch (e) {
    // keep raw value
  }
  console.log('-', k, ':', parsed);
}

// Read a specific key
const user1 = db.get('user:1');
console.log('\nSpecific read for user:1 ->', user1 ? JSON.parse(user1) : 'NOT FOUND');
