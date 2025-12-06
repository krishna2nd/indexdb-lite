#include <nan.h>
#include "local_indexed_db/storage.h"
#include "local_indexed_db/database.h"
#include "local_indexed_db/encoding.h"

using namespace local_indexed_db;

// Forward declarations
class DatabaseWrap;
class TransactionWrap;

// Wrap IDBKey for JavaScript
class IDBKeyWrap : public Nan::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);
  static NAN_NEW(New);

  std::unique_ptr<IDBKey> key_;

 private:
  explicit IDBKeyWrap();
  ~IDBKeyWrap();
};

Nan::Persistent<v8::Function> IDBKeyWrap::constructor;

IDBKeyWrap::IDBKeyWrap() {}
IDBKeyWrap::~IDBKeyWrap() {}

NAN_MODULE_INIT(IDBKeyWrap::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("IDBKey").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  constructor.Reset(tpl->GetFunction(Nan::GetCurrentContext()).ToLocalChecked());
  Nan::Set(target, Nan::New("IDBKey").ToLocalChecked(),
           tpl->GetFunction(Nan::GetCurrentContext()).ToLocalChecked());
}

NAN_NEW(IDBKeyWrap::New) {
  IDBKeyWrap* obj = new IDBKeyWrap();
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

// Database wrapper
class DatabaseWrap : public Nan::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);
  static NAN_METHOD(Put);
  static NAN_METHOD(Get);
  static NAN_METHOD(Delete);
  static NAN_METHOD(CommitBatch);

  std::unique_ptr<Database> db_;

 private:
  DatabaseWrap(std::unique_ptr<IStorage> storage);
  ~DatabaseWrap();
  static NAN_NEW(New);

  static Nan::Persistent<v8::Function> constructor;
};

Nan::Persistent<v8::Function> DatabaseWrap::constructor;

DatabaseWrap::DatabaseWrap(std::unique_ptr<IStorage> storage)
    : db_(std::make_unique<Database>(std::move(storage))) {}
DatabaseWrap::~DatabaseWrap() {}

NAN_MODULE_INIT(DatabaseWrap::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Database").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "put", Put);
  Nan::SetPrototypeMethod(tpl, "get", Get);
  Nan::SetPrototypeMethod(tpl, "delete", Delete);
  Nan::SetPrototypeMethod(tpl, "commitBatch", CommitBatch);

  constructor.Reset(tpl->GetFunction(Nan::GetCurrentContext()).ToLocalChecked());
  Nan::Set(target, Nan::New("Database").ToLocalChecked(),
           tpl->GetFunction(Nan::GetCurrentContext()).ToLocalChecked());
}

NAN_NEW(DatabaseWrap::New) {
  auto storage = CreateInMemoryStorage();
  DatabaseWrap* obj = new DatabaseWrap(std::move(storage));
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(DatabaseWrap::Put) {
  DatabaseWrap* obj = ObjectWrap::Unwrap<DatabaseWrap>(info.This());
  
  if (info.Length() < 2) {
    Nan::ThrowTypeError("Expected 2 arguments");
    return;
  }

  v8::String::Utf8Value key(info.GetIsolate(), info[0]);
  v8::String::Utf8Value value(info.GetIsolate(), info[1]);

  auto status = obj->db_->Put(*key, *value);
  info.GetReturnValue().Set(Nan::New(static_cast<int>(status)));
}

NAN_METHOD(DatabaseWrap::Get) {
  DatabaseWrap* obj = ObjectWrap::Unwrap<DatabaseWrap>(info.This());
  
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Expected 1 argument");
    return;
  }

  v8::String::Utf8Value key(info.GetIsolate(), info[0]);
  std::string value;
  
  auto status = obj->db_->Get(*key, &value);
  
  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  Nan::Set(result, Nan::New("status").ToLocalChecked(), 
           Nan::New(static_cast<int>(status)));
  Nan::Set(result, Nan::New("value").ToLocalChecked(),
           Nan::New(value).ToLocalChecked());
  
  info.GetReturnValue().Set(result);
}

NAN_METHOD(DatabaseWrap::Delete) {
  DatabaseWrap* obj = ObjectWrap::Unwrap<DatabaseWrap>(info.This());
  
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Expected 1 argument");
    return;
  }

  v8::String::Utf8Value key(info.GetIsolate(), info[0]);
  auto status = obj->db_->Delete(*key);
  
  info.GetReturnValue().Set(Nan::New(static_cast<int>(status)));
}

NAN_METHOD(DatabaseWrap::CommitBatch) {
  DatabaseWrap* obj = ObjectWrap::Unwrap<DatabaseWrap>(info.This());
  
  if (info.Length() < 1 || !info[0]->IsArray()) {
    Nan::ThrowTypeError("Expected array of [key, value] pairs");
    return;
  }

  v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(info[0]);
  std::vector<std::pair<std::string, std::optional<std::string>>> batch;
  
  for (uint32_t i = 0; i < arr->Length(); ++i) {
    v8::Local<v8::Value> item = Nan::Get(arr, i).ToLocalChecked();
    if (!item->IsArray()) {
      Nan::ThrowTypeError("Expected array of [key, value] pairs");
      return;
    }
    
    v8::Local<v8::Array> pair = v8::Local<v8::Array>::Cast(item);
    if (pair->Length() < 1) {
      Nan::ThrowTypeError("Pair must have at least key");
      return;
    }
    
    v8::String::Utf8Value key(info.GetIsolate(), Nan::Get(pair, 0).ToLocalChecked());
    v8::Local<v8::Value> val = Nan::Get(pair, 1).ToLocalChecked();
    
    if (val->IsNull() || val->IsUndefined()) {
      batch.emplace_back(*key, std::nullopt);
    } else {
      v8::String::Utf8Value value(info.GetIsolate(), val);
      batch.emplace_back(*key, std::optional<std::string>(*value));
    }
  }
  
  auto status = obj->db_->CommitBatch(batch);
  info.GetReturnValue().Set(Nan::New(static_cast<int>(status)));
}

// Module initialization
NAN_MODULE_INIT(InitModule) {
  IDBKeyWrap::Init(target);
  DatabaseWrap::Init(target);
}

NODE_MODULE(local_indexed_db_native, InitModule)
