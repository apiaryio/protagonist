#include <string>
#include <sstream>
#include "v8_wrapper.h"
#include "protagonist.h"
#include "snowcrash.h"

using std::string;
using namespace v8;
using namespace protagonist;

// Async Validate
void AsyncValidate(uv_work_t* request);

// Async Validate Handler
void AsyncValidateAfter(uv_work_t* request);

// Threadpooling libuv baton
struct ValidateBaton {

    // Callback
    Nan::Persistent<Function> callback;

    // Input
    snowcrash::BlueprintParserOptions options;
    mdp::ByteBuffer sourceData;

    // Output
    snowcrash::ParseResult<snowcrash::Blueprint> parseResult;
    sos::Object result;
};

NAN_METHOD(protagonist::Validate) {
    Nan::HandleScope scope;

    // Check arguments
    if (info.Length() != 2 && info.Length() != 3) {
        Nan::ThrowTypeError("wrong number of arguments, `validate(string, options, callback)` expected");
        return;
    }

    if (!info[0]->IsString()) {
        Nan::ThrowTypeError("wrong argument - string expected, `validate(string, options, callback)`");
        return;
    }

    if ((info.Length() == 2 && !info[1]->IsFunction()) ||
        (info.Length() == 3 && !info[2]->IsFunction())) {

        Nan::ThrowTypeError("wrong argument - callback expected, `validate(string, options, callback)`");
        return;
    }

    if (info.Length() == 3 && !info[1]->IsObject()) {
        Nan::ThrowTypeError("wrong argument - object expected, `validate(string, options, callback)`");
        return;
    }

    // Get source data
    String::Utf8Value sourceData(info[0]->ToString());

    // Prepare options
    snowcrash::BlueprintParserOptions options = 0;

    if (info.Length() == 3) {
        OptionsResult *optionsResult = ParseOptionsObject(Handle<Object>::Cast(info[1]), true);

        if (optionsResult->error != NULL) {
            Nan::ThrowTypeError(optionsResult->error);
            return;
        }

        options = optionsResult->options;
        free(optionsResult);
    }

    // Get Callback
    Local<Function> callback = (info.Length() == 3) ? Local<Function>::Cast(info[2]) : Local<Function>::Cast(info[1]);

    // Prepare threadpool baton
    ValidateBaton* baton = ::new ValidateBaton();
    baton->options = options;
    baton->sourceData = *sourceData;
    baton->callback.Reset(callback);

    // This creates the work request struct.
    uv_work_t *request = ::new uv_work_t();
    request->data = baton;

    // Schedule the work request
    int status = uv_queue_work(uv_default_loop(),
                                request,
                                AsyncValidate,
                                (uv_after_work_cb)AsyncValidateAfter);

    assert(status == 0);
    return;
}

void AsyncValidate(uv_work_t* request) {
    ValidateBaton* baton = static_cast<ValidateBaton*>(request->data);

    baton->result = Validate::Do(baton->sourceData, baton->options);
}

void AsyncValidateAfter(uv_work_t* request) {
    Nan::HandleScope scope;
    ValidateBaton* baton = static_cast<ValidateBaton*>(request->data);

    // Prepare report
    const unsigned argc = 2;
    Local<Value> argv[argc];

    // Error Object
    argv[0] = Nan::Null();

    if (baton->result.empty())
        argv[1] = Nan::Null();
    else
        argv[1] = v8_wrap(baton->result)->ToObject();

    TryCatch try_catch;
    Local<Function> callback = Nan::New<Function>(baton->callback);
    callback->Call(Nan::GetCurrentContext()->Global(), argc, argv);

    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }

    baton->callback.Reset();
    delete baton;
    delete request;
}
