const { Database, IDBKey, Encoding, Status } = require('../lib/index');
const assert = require('assert');

describe('Local Indexed DB Node.js Binding', () => {
  let db;

  beforeEach(() => {
    db = new Database();
  });

  describe('Database.put() and Database.get()', () => {
    it('should store and retrieve a simple string value', () => {
      const status = db.put('key1', 'value1');
      assert.strictEqual(status, Status.OK);

      const result = db.get('key1');
      assert.strictEqual(result.status, Status.OK);
      assert.strictEqual(result.value, 'value1');
    });

    it('should return NOT_FOUND for missing keys', () => {
      const result = db.get('nonexistent');
      assert.strictEqual(result.status, Status.NOT_FOUND);
    });

    it('should overwrite existing values', () => {
      db.put('key1', 'value1');
      db.put('key1', 'value2');

      const result = db.get('key1');
      assert.strictEqual(result.value, 'value2');
    });
  });

  describe('Database.delete()', () => {
    it('should delete keys', () => {
      db.put('key1', 'value1');
      assert.strictEqual(db.delete('key1'), Status.OK);

      const result = db.get('key1');
      assert.strictEqual(result.status, Status.NOT_FOUND);
    });
  });

  describe('Database.commitBatch()', () => {
    it('should commit multiple operations atomically', () => {
      const batch = [
        ['batch_key_1', 'batch_value_1'],
        ['batch_key_2', 'batch_value_2'],
        ['batch_key_3', 'batch_value_3']
      ];

      const status = db.commitBatch(batch);
      assert.strictEqual(status, Status.OK);

      const result1 = db.get('batch_key_1');
      assert.strictEqual(result1.value, 'batch_value_1');

      const result2 = db.get('batch_key_2');
      assert.strictEqual(result2.value, 'batch_value_2');
    });

    it('should handle null values (deletes) in batch', () => {
      db.put('key_to_delete', 'value');
      const batch = [
        ['key_to_delete', null],
        ['new_key', 'new_value']
      ];

      db.commitBatch(batch);

      const deletedResult = db.get('key_to_delete');
      assert.strictEqual(deletedResult.status, Status.NOT_FOUND);

      const newResult = db.get('new_key');
      assert.strictEqual(newResult.value, 'new_value');
    });
  });

  describe('IDBKey', () => {
    it('should create keys of different types', () => {
      const nullKey = IDBKey.null();
      assert.strictEqual(nullKey.type, IDBKey.TYPES.NULL);

      const numberKey = new IDBKey(42.5);
      assert.strictEqual(numberKey.type, IDBKey.TYPES.NUMBER);

      const stringKey = new IDBKey('abc');
      assert.strictEqual(stringKey.type, IDBKey.TYPES.STRING);

      const dateKey = IDBKey.date(1609459200000);
      assert.strictEqual(dateKey.type, IDBKey.TYPES.DATE);
    });

    it('should detect array keys', () => {
      const arrayKey = new IDBKey([1, 'two', null]);
      assert.strictEqual(arrayKey.type, IDBKey.TYPES.ARRAY);
    });
  });

  describe('Encoding utilities', () => {
    it('should encode and decode keys', () => {
      const originalKey = new IDBKey('test_value');
      const encoded = Encoding.encodeKey(originalKey);
      const decoded = Encoding.decodeKey(encoded);

      assert.strictEqual(decoded.type, IDBKey.TYPES.STRING);
      assert.strictEqual(decoded.value, 'test_value');
    });

    it('should handle encoding of various types', () => {
      const keys = [
        new IDBKey(123),
        new IDBKey('string'),
        IDBKey.null(),
        IDBKey.date(1000)
      ];

      for (const key of keys) {
        const encoded = Encoding.encodeKey(key);
        const decoded = Encoding.decodeKey(encoded);
        assert.strictEqual(decoded.type, key.type);
      }
    });
  });

  describe('Helper methods', () => {
    it('should check status with Database.isOk()', () => {
      const result = db.get('nonexistent');
      assert.strictEqual(Database.isOk(result), false);
      assert.strictEqual(Database.isNotFound(result), true);

      db.put('test', 'value');
      const result2 = db.get('test');
      assert.strictEqual(Database.isOk(result2), true);
      assert.strictEqual(Database.isNotFound(result2), false);
    });
  });
});
