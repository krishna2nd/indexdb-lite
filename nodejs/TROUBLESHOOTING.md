# Troubleshooting & FAQ

## Frequently Asked Questions

### Q: What Node.js versions are supported?
**A:** Node.js 14.x and higher (tested with 14, 16, 18). The NaN addon framework ensures compatibility across versions.

### Q: Is the data persisted?
**A:** No, the default backend is in-memory. All data is lost when the process exits. To add file persistence, implement a custom IStorage backend in the portable library and recompile the binding.

### Q: Can I use this in production?
**A:** The module is feature-complete but currently uses in-memory storage. For production use:
1. Implement a file-backed IStorage backend (LevelDB, RocksDB, etc.)
2. Run extensive performance testing with your workload
3. Add error recovery and persistence validation
4. Consider async API for non-blocking operations

### Q: Are operations synchronous or asynchronous?
**A:** Operations are fully synchronous (block until complete). The binding uses the NaN framework's synchronous API. For non-blocking use in event-driven systems, wrap operations with `setImmediate()` or use a thread pool library.

### Q: What's the license?
**A:** See the LICENSE file in the parent directory. The portable C++ library is derived from Chromium's IndexedDB implementation.

### Q: Can I use this with TypeScript?
**A:** Yes. Create a type definition file:
```typescript
// index.d.ts
declare module 'local_indexed_db' {
  class Database {
    put(key: string, value: string): number;
    get(key: string): string | number;
    delete(key: string): number;
    commitBatch(batch: WriteBatch): number;
    WriteBatch: new () => WriteBatch;
  }
  
  interface WriteBatch {
    put(key: string, value: string): void;
    delete(key: string): void;
  }
  
  class IDBKey {
    static null(): IDBKey;
    static number(n: number): IDBKey;
    static string(s: string): IDBKey;
    static date(ms: number): IDBKey;
    static array(arr: IDBKey[]): IDBKey;
    getType(): string;
  }
  
  const Status: {
    OK: number;
    NOT_FOUND: number;
    ERROR: number;
    isOk(s: number): boolean;
    isNotFound(s: number): boolean;
    isError(s: number): boolean;
  };
  
  const Encoding: {
    encodeKey(key: any): string;
    decodeKey(encoded: string): any;
  };
  
  export { Database, IDBKey, Encoding, Status };
}
```

Then use:
```typescript
import { Database, Status } from 'local_indexed_db';

const db = new Database();
const status: number = db.put('key', 'value');
if (Status.isOk(status)) {
  console.log('Success');
}
```

### Q: How do I handle concurrent access?
**A:** The in-memory backend is thread-safe (uses mutex). However, Node.js runs single-threaded by default. For multi-threaded access, use worker threads and ensure each worker has its own Database instance, or implement a connection pool with proper locking.

### Q: Can I query by range or prefix?
**A:** Not in the current binding. The API only supports exact key lookups. To add range queries:
1. Implement the Cursor interface in the portable library
2. Expose it through the NaN binding
3. Recompile

The underlying portable library supports range queries via ObjectStoreDataKey prefix encoding.

---

## Troubleshooting

### Build & Installation Issues

#### Error: "python not found"

**Symptom:**
```
gyp ERR! find Python
gyp ERR! error: Can't find Python executable "python", you can set the PYTHON env variable.
```

**Solution:**
```bash
# Install Python 3
# Ubuntu/Debian
sudo apt-get install python3

# macOS
brew install python3

# Set PYTHON environment variable
export PYTHON=/usr/bin/python3
npm run build

# Or specify during install
npm config set python /usr/bin/python3
npm install
```

#### Error: "No build tools found"

**Symptom:**
```
gyp ERR! build error
gyp ERR! stack Error: Command failed: make
gyp ERR! stack ChildProcessError: running `make` failed with:
```

**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# macOS
xcode-select --install

# Windows
# Download and install Visual Studio Build Tools 2015 or later
# from https://visualstudio.microsoft.com/downloads/
```

#### Error: "node.h not found"

**Symptom:**
```
fatal error: node.h: No such file or directory
```

**Solution:**
```bash
# Reinstall node-gyp and rebuild
npm install -g node-gyp
npm run build -- --nodedir=/usr/include/node

# Or reinstall Node.js headers
npm install --save-dev node-gyp
npm run build
```

#### Error: "C++ version too old"

**Symptom:**
```
error: 'optional' is not a member of 'std'
```

**Solution:**
The module requires C++17. Update your build tools:

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install g++-9

# Specify compiler
npm run build -- --cc=g++-9 --cxx=g++-9

# macOS
brew install gcc@9
npm run build
```

---

### Runtime Issues

#### Error: "Module not found: local_indexed_db_native"

**Symptom:**
```
Error: Cannot find module '../build/Release/local_indexed_db_native.node'
```

**Solution:**
```bash
# Ensure native module was built
ls -la build/Release/

# If missing, rebuild
npm run build

# Check node_modules integrity
npm ls nan

# If problems persist, clean and rebuild
rm -rf build node_modules
npm install
npm run build
```

#### Error: "Database is not a constructor"

**Symptom:**
```javascript
const Database = require('./lib/index.js'); // ❌ Wrong
const db = new Database(); // Error!
```

**Solution:**
```javascript
// ✅ Correct
const { Database } = require('./lib/index.js');
const db = new Database();

// OR
const module = require('./lib/index.js');
const db = new module.Database();
```

#### Operations return ERROR status (2)

**Common causes:**
1. **Invalid key/value types** - Must be strings
2. **Encoding issues** - Non-UTF8 data
3. **Memory corruption** - Rare; indicates binding bug

**Solution:**
```javascript
// ❌ Wrong
db.put('key', { obj: 'value' }); // Objects not allowed

// ✅ Correct
db.put('key', JSON.stringify({ obj: 'value' }));

// Verify operation
const status = db.put('key', 'value');
if (!Status.isOk(status)) {
  console.error('Put failed with status:', status);
}
```

#### db.get() returns NOT_FOUND but key should exist

**Possible causes:**
1. **Key was never inserted** - Check earlier put() calls
2. **Type mismatch** - Keys are case-sensitive strings
3. **Batch not committed** - Uncommitted batches don't affect database state

**Solution:**
```javascript
// Verify key exists before reading
const val = db.get('myKey');

if (Status.isNotFound(val)) {
  console.log('Key not in database');
  // Try putting first
  db.put('myKey', 'value');
  const val2 = db.get('myKey');
  console.log('After put:', val2);
}
```

#### Tests fail to run

**Symptom:**
```
Error: Cannot find module '../build/Release/local_indexed_db_native.node'
```

**Solution:**
```bash
# Build must complete before testing
npm run build
npm test

# If test still fails, check test file can load
node -e "require('./lib/index.js'); console.log('Module OK')"
```

#### Memory grows without bound

**Symptom:** Process memory keeps increasing; `top` or `htop` shows growing RSS

**Cause:** In-memory backend stores all data in process memory. Expected behavior.

**Solution:**
1. **For testing:** Use smaller datasets
2. **For production:** Implement file-backed storage backend
3. **For demos:** Periodically delete old records:
   ```javascript
   // Cleanup old entries
   for (let i = 0; i < 1000; i++) {
     db.delete(`old_key_${i}`);
   }
   ```

#### Intermittent crashes or segfaults

**Symptoms:**
```
Segmentation fault (core dumped)
or
FATAL ERROR: noncopyable assignment operator
```

**Likely causes:**
1. Memory corruption in binding layer
2. Use-after-free in NaN wrapper
3. Missing include guards in headers

**Debug steps:**
```bash
# Run under gdb to get backtrace
gdb --args node example.js

# At gdb prompt:
# run
# (wait for crash)
# bt
# quit
```

**Report to maintainers with:**
- gdb backtrace
- Node.js version
- Platform (Linux/macOS/Windows)
- Steps to reproduce

---

### Performance Issues

#### Database operations are slow

**Symptom:** put/get/delete take > 10ms per operation

**Likely causes:**
1. System is under heavy load
2. Large objects being stringified
3. Many pending operations

**Profiling:**
```javascript
const { Database, Status } = require('./lib/index.js');
const db = new Database();

// Benchmark
const iterations = 1000;
const start = Date.now();

for (let i = 0; i < iterations; i++) {
  db.put(`key_${i}`, `value_${i}`);
}

const elapsed = Date.now() - start;
console.log(`${iterations} puts: ${elapsed}ms (${(elapsed/iterations).toFixed(3)}ms each)`);
```

**Expected:** < 1ms per operation

**If slower:**
- Check system load: `top` or `htop`
- Reduce object size in values
- Use batches for bulk operations

#### Memory usage is high

**Symptom:** Process uses > 500MB for small dataset

**Likely cause:** In-memory backend retains all data

**Solution:**
1. Reduce dataset size
2. Implement LevelDB backend (extends portable library)
3. Switch to production database (MongoDB, PostgreSQL, etc.)

---

## Advanced Troubleshooting

### Building with custom portable library changes

If you modify files in `../portable/`, rebuild the binding:

```bash
# The binding links portable sources statically
# Changing portable code requires recompile

# Clean and rebuild
npm run build -- --clean
npm run build

# Or use full clean
rm -rf build
npm run build
```

### Debugging the native binding

Create `debug_binding.js`:

```javascript
try {
  const binding = require('../build/Release/local_indexed_db_native.node');
  console.log('Binding exports:', Object.keys(binding));
  console.log('Database:', typeof binding.Database);
  
  // Test instantiation
  const db = new binding.Database();
  console.log('Database instance created:', typeof db);
  
  // Test basic operation
  const status = db.put('test', 'value');
  console.log('Put status:', status);
  
  const value = db.get('test');
  console.log('Get value:', value);
  
} catch (e) {
  console.error('Binding error:', e.message);
  console.error('Stack:', e.stack);
}
```

Run it:
```bash
node debug_binding.js
```

### Checking node-gyp configuration

```bash
# View current config
node-gyp configure --verbose

# View build config
cat build/config.gypi

# Check Python version used
python --version  # or python3 --version
```

### Examining generated files

After build, inspect:

```bash
# Generated build files
ls -la build/

# Build configuration
cat build/config.gypi

# Compiler output (if verbose build)
npm run build -- --verbose > build.log 2>&1
less build.log
```

---

## Reporting Issues

When reporting problems, include:

1. **Environment:**
   - Node.js version: `node --version`
   - npm version: `npm --version`
   - Python version: `python --version`
   - OS and architecture: `uname -m`

2. **Steps to reproduce:**
   - Exact commands run
   - Input data (if relevant)
   - Full error message

3. **Artifacts:**
   - `npm run build -- --verbose` output
   - Any gdb backtrace (from segfaults)
   - Value of `process.versions`

4. **Example that fails:**
   - Minimal code to reproduce the issue
   - Expected vs actual behavior

---

## Getting Help

1. **Check this guide** - Most issues are covered above
2. **Review tests** - `test/database.test.js` shows correct usage
3. **Run example** - `node example.js` demonstrates features
4. **Read API docs** - `README.md` and `SETUP.md` have detailed reference
5. **Check portable library** - For advanced: see `../portable/README.md`

