#!/usr/bin/env node
const fs = require('fs');
const path = require('path');

// Try to load the published package; if native binding isn't available
// fall back to a small file-backed JS implementation that exposes the
// same minimal API used in this example (put, get, delete, commitBatch).
let PackageLib;
try {
  PackageLib = require('indexdb-lite');
} catch (err) {
  // Fallback implementation
  PackageLib = null;
}

const DB_FILE = path.resolve(__dirname, '../data/npm_company.db');

class FallbackDB {
  constructor(filePath = DB_FILE) {
    this.filePath = filePath;
    this._load();
  }

  _load() {
    try {
      const raw = fs.readFileSync(this.filePath, 'utf8');
      this.store = JSON.parse(raw);
    } catch (e) {
      this.store = {}; // key -> string value
    }
  }

  _save() {
    fs.mkdirSync(path.dirname(this.filePath), { recursive: true });
    fs.writeFileSync(this.filePath, JSON.stringify(this.store, null, 2), 'utf8');
  }

  put(key, value) {
    this.store[key] = value;
    this._save();
    return 0; // Status OK
  }

  get(key) {
    if (Object.prototype.hasOwnProperty.call(this.store, key)) {
      return { status: 0, value: this.store[key] };
    }
    return { status: 1, value: null };
  }

  delete(key) {
    if (Object.prototype.hasOwnProperty.call(this.store, key)) {
      delete this.store[key];
      this._save();
      return 0;
    }
    return 1;
  }

  commitBatch(batch) {
    // batch: Array<[key, value|null]> where value===null means delete
    for (const [k, v] of batch) {
      if (v === null) delete this.store[k];
      else this.store[k] = v;
    }
    this._save();
    return 0;
  }
}

function createDatabase() {
  if (PackageLib && PackageLib.Database) {
    try {
      return new PackageLib.Database();
    } catch (e) {
      // Fallthrough to fallback
    }
  }
  return new FallbackDB(DB_FILE);
}

const db = createDatabase();

// Helpers
const keyFor = (collection, id) => `${collection}:${id}`;

async function addDepartment(dept) {
  const key = keyFor('departments', dept.id);
  return db.put(key, JSON.stringify(dept));
}

async function addEmployee(emp) {
  const key = keyFor('employees', emp.id);
  return db.put(key, JSON.stringify(emp));
}

function getRaw(key) {
  const res = db.get(key);
  if (res && res.status === 0) return res.value;
  return null;
}

function getItem(collection, id) {
  const raw = getRaw(keyFor(collection, id));
  return raw ? JSON.parse(raw) : null;
}

function getAll(collection) {
  // For the fallback we scan the JSON store; for native DB this assumes
  // that keys are simple strings and we can iterate by known ids — since
  // the native binding doesn't expose iteration in this example, we will
  // attempt to read a persisted file path if using fallback only.
  if (db instanceof FallbackDB) {
    return Object.keys(db.store)
      .filter(k => k.startsWith(collection + ':'))
      .map(k => JSON.parse(db.store[k]));
  }

  // Best-effort for native binding: try a conventionally stored manifest
  const manifestRaw = getRaw('__manifest__');
  if (manifestRaw) {
    const manifest = JSON.parse(manifestRaw);
    const ids = manifest[collection] || [];
    return ids.map(id => getItem(collection, id)).filter(Boolean);
  }

  return [];
}

function getEmployeeWithDepartment(empId) {
  const emp = getItem('employees', empId);
  if (!emp) return null;
  const dept = getItem('departments', emp.departmentId) || null;
  return { ...emp, department: dept };
}

function getDepartmentWithEmployees(deptId) {
  const dept = getItem('departments', deptId);
  if (!dept) return null;
  const employees = getAll('employees').filter(e => e.departmentId === deptId);
  return { ...dept, employees };
}

async function seedDemo() {
  // Departments
  const departments = [
    { id: 'DEPT001', name: 'Engineering', head: 'Alice Johnson', budget: 500000 },
    { id: 'DEPT002', name: 'Sales', head: 'Bob Smith', budget: 300000 },
    { id: 'DEPT003', name: 'HR', head: 'Carol Williams', budget: 150000 },
    { id: 'DEPT004', name: 'Finance', head: 'David Brown', budget: 200000 }
  ];

  for (const d of departments) await addDepartment(d);

  // Employees
  const employees = [
    { id: 'EMP001', name: 'John Doe', salary: 120000, position: 'Senior Engineer', departmentId: 'DEPT001' },
    { id: 'EMP002', name: 'Jane Smith', salary: 95000, position: 'Engineer', departmentId: 'DEPT001' },
    { id: 'EMP003', name: 'Mike Johnson', salary: 80000, position: 'Engineer', departmentId: 'DEPT001' },
    { id: 'EMP004', name: 'Sarah Davis', salary: 70000, position: 'Sales Rep', departmentId: 'DEPT002' },
    { id: 'EMP005', name: 'Tom Clark', salary: 75000, position: 'HR Specialist', departmentId: 'DEPT003' },
    { id: 'EMP006', name: 'Lisa Anderson', salary: 68000, position: 'Accountant', departmentId: 'DEPT004' }
  ];

  for (const e of employees) await addEmployee(e);

  // If using fallback, also write a manifest to help iteration for native fallback parity
  if (db instanceof FallbackDB) {
    const manifest = { departments: departments.map(d => d.id), employees: employees.map(e => e.id) };
    db.put('__manifest__', JSON.stringify(manifest));
  }
}

function printRow(obj) {
  console.log(JSON.stringify(obj, null, 2));
}

async function runDemo() {
  console.log('Using', PackageLib ? 'published indexdb-lite package' : 'fallback JS implementation');
  console.log('Database file:', DB_FILE);

  await seedDemo();

  console.log('\n-- Single employee with department (EMP002) --');
  printRow(getEmployeeWithDepartment('EMP002'));

  console.log('\n-- All employees with department --');
  for (const e of getAll('employees')) {
    printRow(getEmployeeWithDepartment(e.id));
  }

  console.log('\n-- Single department with employees (DEPT001) --');
  printRow(getDepartmentWithEmployees('DEPT001'));

  console.log('\n-- All departments with employees --');
  for (const d of getAll('departments')) {
    printRow(getDepartmentWithEmployees(d.id));
  }
}

module.exports = { runDemo, getEmployeeWithDepartment, getDepartmentWithEmployees };

if (require.main === module) {
  runDemo().catch(err => {
    console.error('Demo failed:', err);
    process.exit(1);
  });
}
