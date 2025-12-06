// Simple file-backed key-value DB helper
// Uses an atomic write (tmp file + rename) to persist a JSON object to disk.

const fs = require('fs');
const path = require('path');

class FileDB {
  constructor(filePath) {
    this.filePath = path.resolve(filePath);
    const dir = path.dirname(this.filePath);
    if (!fs.existsSync(dir)) fs.mkdirSync(dir, { recursive: true });
    if (!fs.existsSync(this.filePath)) fs.writeFileSync(this.filePath, JSON.stringify({}), 'utf8');
  }

  _read() {
    try {
      const raw = fs.readFileSync(this.filePath, 'utf8');
      return raw ? JSON.parse(raw) : {};
    } catch (e) {
      // If parse fails, return empty object to avoid crashes
      return {};
    }
  }

  _write(obj) {
    const tmp = this.filePath + '.tmp';
    fs.writeFileSync(tmp, JSON.stringify(obj, null, 2), 'utf8');
    fs.renameSync(tmp, this.filePath);
  }

  // Synchronous put
  put(key, value) {
    const db = this._read();
    db[key] = value;
    this._write(db);
  }

  // Synchronous get, returns null if missing
  get(key) {
    const db = this._read();
    return Object.prototype.hasOwnProperty.call(db, key) ? db[key] : null;
  }

  // Delete key, return true if deleted
  delete(key) {
    const db = this._read();
    if (Object.prototype.hasOwnProperty.call(db, key)) {
      delete db[key];
      this._write(db);
      return true;
    }
    return false;
  }

  // List keys
  keys() {
    return Object.keys(this._read());
  }
}

module.exports = FileDB;
