const FileDB = require('./file_db');

// Writes sample data to sample-index.db in the current directory
const db = new FileDB('sample-index.db');

console.log('Writing data to sample-index.db...');

db.put('user:1', JSON.stringify({ id: 1, name: 'Alice', email: 'alice@example.com' }));
db.put('user:2', JSON.stringify({ id: 2, name: 'Bob', email: 'bob@example.com' }));

db.put('config:lastUpdated', new Date().toISOString());

console.log('Wrote keys:', db.keys().join(', '));
console.log('Done.');
