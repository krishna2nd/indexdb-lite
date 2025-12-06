// JavaScript wrapper for local-indexed-db native module

const binding = require('../build/Release/local_indexed_db_native');

/**
 * Status constants
 */
const Status = {
  OK: 0,
  NOT_FOUND: 1,
  ERROR: 2
};

/**
 * Database class wrapping the native C++ implementation
 */
class Database {
  constructor() {
    this.db = new binding.Database();
  }

  /**
   * Store a key-value pair
   * @param {string} key
   * @param {string} value
   * @returns {number} status code
   */
  put(key, value) {
    return this.db.put(key, value);
  }

  /**
   * Retrieve a value by key
   * @param {string} key
   * @returns {{status: number, value: string}} result with status and value
   */
  get(key) {
    return this.db.get(key);
  }

  /**
   * Delete a key-value pair
   * @param {string} key
   * @returns {number} status code
   */
  delete(key) {
    return this.db.delete(key);
  }

  /**
   * Commit a batch of writes atomically
   * @param {Array<[string, string|null]>} batch array of [key, value] pairs
   * @returns {number} status code
   */
  commitBatch(batch) {
    return this.db.commitBatch(batch);
  }

  /**
   * Check if a get operation succeeded
   * @param {Object} result from get()
   * @returns {boolean}
   */
  static isOk(result) {
    return result && result.status === Status.OK;
  }

  /**
   * Check if key not found
   * @param {Object} result from get()
   * @returns {boolean}
   */
  static isNotFound(result) {
    return result && result.status === Status.NOT_FOUND;
  }
}

/**
 * IDBKey class for encoding IndexedDB keys
 */
class IDBKey {
  static TYPES = {
    INVALID: 0,
    NULL: 1,
    NUMBER: 2,
    DATE: 3,
    STRING: 4,
    BINARY: 5,
    ARRAY: 6
  };

  constructor(value) {
    this.value = value;
    this.type = IDBKey.getType(value);
  }

  static getType(value) {
    if (value === null) return IDBKey.TYPES.NULL;
    if (typeof value === 'number') return IDBKey.TYPES.NUMBER;
    if (typeof value === 'string') return IDBKey.TYPES.STRING;
    if (value instanceof Date) return IDBKey.TYPES.DATE;
    if (Buffer.isBuffer(value)) return IDBKey.TYPES.BINARY;
    if (Array.isArray(value)) return IDBKey.TYPES.ARRAY;
    return IDBKey.TYPES.INVALID;
  }

  /**
   * Create a null key
   */
  static null() {
    return new IDBKey(null);
  }

  /**
   * Create a date key (stored as ms since epoch)
   */
  static date(ms) {
    const key = new IDBKey(ms);
    key.isDate = true;
    return key;
  }
}

/**
 * Helper utilities for encoding/decoding
 */
class Encoding {
  /**
   * Encode an IDBKey to a buffer for storage
   * @param {IDBKey|*} key
   * @returns {Buffer} encoded key
   */
  static encodeKey(key) {
    if (!(key instanceof IDBKey)) {
      key = new IDBKey(key);
    }

    // Simple encoding: type byte + JSON payload
    // In production, would use the C++ sortable encoding
    const payload = JSON.stringify({
      type: key.type,
      value: key.value
    });

    return Buffer.from(payload);
  }

  /**
   * Decode a buffer back to an IDBKey
   * @param {Buffer} encoded
   * @returns {IDBKey}
   */
  static decodeKey(encoded) {
    try {
      const data = JSON.parse(encoded.toString());
      const key = new IDBKey(data.value);
      key.type = data.type;
      return key;
    } catch (e) {
      return IDBKey.null();
    }
  }
}

module.exports = {
  Database,
  IDBKey,
  Encoding,
  Status
};
