/**
 * local-indexed-db Type Definitions
 * 
 * TypeScript definitions for the Node.js binding of the portable
 * Local Indexed DB C++ library.
 */

export interface StatusResult {
  status: number;
  value?: string;
}

/**
 * Status codes for database operations.
 */
export namespace Status {
  const OK: number;
  const NOT_FOUND: number;
  const ERROR: number;
}

/**
 * Main Database class for key-value operations.
 */
export class Database {
  /**
   * Create a new in-memory database instance.
   */
  constructor();

  /**
   * Store a key-value pair.
   * @param key The key to store
   * @param value The value to store (typically JSON string)
   * @returns Status code (0 = OK)
   */
  put(key: string, value: string): number;

  /**
   * Retrieve a value by key.
   * @param key The key to retrieve
   * @returns Object with {status, value} where status is 0 (OK), 1 (NOT_FOUND), or 2 (ERROR)
   */
  get(key: string): StatusResult;

  /**
   * Delete a key-value pair.
   * @param key The key to delete
   * @returns Status code (0 = OK)
   */
  delete(key: string): number;

  /**
   * Commit multiple operations atomically.
   * @param batch Array of [key, value] tuples; null value = delete
   * @returns Status code (0 = OK)
   * 
   * @example
   * const batch = [
   *   ['user:1', JSON.stringify({id:1,name:'Alice'})],
   *   ['user:2', JSON.stringify({id:2,name:'Bob'})],
   *   ['old:key', null]  // delete
   * ];
   * db.commitBatch(batch);
   */
  commitBatch(batch: Array<[string, string | null]>): number;

  /**
   * Check if a get() result is successful.
   * @param result The result from get()
   * @returns true if status is OK, false otherwise
   */
  static isOk(result: StatusResult): boolean;

  /**
   * Check if a get() result indicates key not found.
   * @param result The result from get()
   * @returns true if status is NOT_FOUND, false otherwise
   */
  static isNotFound(result: StatusResult): boolean;

  /**
   * Check if a get() result indicates an error.
   * @param result The result from get()
   * @returns true if status is ERROR, false otherwise
   */
  static isError(result: StatusResult): boolean;
}

/**
 * IDBKey type enumeration.
 */
export namespace IDBKey {
  const TYPES: {
    INVALID: number;
    NULL: number;
    NUMBER: number;
    DATE: number;
    STRING: number;
    BINARY: number;
    ARRAY: number;
  };

  /**
   * Create a null key.
   * @returns An IDBKey representing null
   */
  function null(): IDBKey;

  /**
   * Create a date key.
   * @param ms Milliseconds since epoch
   * @returns An IDBKey representing a date
   */
  function date(ms: number): IDBKey;
}

/**
 * IndexedDB Key wrapper for type handling.
 */
export class IDBKey {
  /**
   * Create a key from a value. Automatically detects type.
   * @param value The value to wrap (number, string, Date, array, etc.)
   */
  constructor(value: any);

  /**
   * The type of the key (see IDBKey.TYPES)
   */
  type: number;

  /**
   * The underlying value
   */
  value: any;
}

/**
 * Encoding utilities for keys and values.
 */
export namespace Encoding {
  /**
   * Encode an IDBKey to a Buffer.
   * @param key The IDBKey to encode
   * @returns A Buffer with the encoded key
   */
  function encodeKey(key: IDBKey): Buffer;

  /**
   * Decode a Buffer back to an IDBKey.
   * @param buffer The Buffer to decode
   * @returns An IDBKey
   */
  function decodeKey(buffer: Buffer): IDBKey;

  /**
   * Encode a key path (used in IndexedDB key paths).
   * @param keyPath The key path to encode
   * @returns A Buffer with the encoded key path
   */
  function encodeKeyPath(keyPath: string): Buffer;

  /**
   * Decode a key path.
   * @param buffer The Buffer to decode
   * @returns A string representing the key path
   */
  function decodeKeyPath(buffer: Buffer): string;
}

/**
 * Exported factory and helpers from lib/index.js
 */
export {
  Database,
  IDBKey,
  Encoding,
  Status
};
