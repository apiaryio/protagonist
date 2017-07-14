#include <string>
#include "protagonist.h"
#include "refractToV8.h"

using std::string;
using namespace v8;
using namespace protagonist;

// Async Validate
void AsyncValidate(uv_work_t* request);

// Async Validate Handler
void AsyncValidateAfter(uv_work_t* request);

// Threadpooling libuv baton
struct ValidateBaton {

    ~ValidateBaton(){
        if (sourceData) {
            delete sourceData;
        }
        if (result) {
            drafter_free_result(result);
        }
    }
    // Callback
    Nan::Persistent<Function> callback;

    // Input
    drafter_parse_options parseOptions;
    Nan::Utf8String *sourceData;

    // Output
    drafter_result *result;
    int parse_err_code;
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
    drafter_parse_options parseOptions = {false};

    if (info.Length() == 3) {
        OptionsResult *optionsResult = ParseOptionsObject(Handle<Object>::Cast(info[1]), true);

        if (optionsResult->error != NULL) {
            Nan::ThrowTypeError(optionsResult->error);
        }

        parseOptions = optionsResult->parseOptions;
        FreeOptionsResult(&optionsResult);
    }

    // Get Callback
    Local<Function> callback = (info.Length() == 3) ? Local<Function>::Cast(info[2]) : Local<Function>::Cast(info[1]);

    // Prepare threadpool baton
    ValidateBaton* baton = ::new ValidateBaton();
    baton->parseOptions = parseOptions;
    baton->sourceData = new Nan::Utf8String(info[0]);
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

    baton->parse_err_code = drafter_check_blueprint(*(*baton->sourceData),
                                                    &baton->result,
                                                    baton->parseOptions);
}

void AsyncValidateAfter(uv_work_t* request) {
    Nan::HandleScope scope;
    ValidateBaton* baton = static_cast<ValidateBaton*>(request->data);

    // Prepare report
    const unsigned argc = 2;
    Local<Value> argv[argc];

    // Error Object
    switch (baton->parse_err_code) {
    case DRAFTER_EUNKNOWN:
        argv[0] = Nan::Error("Parser: Unknown Error");
        argv[1] = Nan::Null();
        break;
    case DRAFTER_EINVALID_INPUT:
        argv[0] = Nan::Error("Parser: Invalid Input");
        argv[1] = Nan::Null();
        break;
    case DRAFTER_EINVALID_OUTPUT:
        argv[0] = Nan::Error("Parser: Invalid Output");
        argv[1] = Nan::Null();
        break;
    default:
        argv[0] = Nan::Null();
        if (baton->result) {
            argv[1] = refract2v8(baton->result, {true, DRAFTER_SERIALIZE_JSON});
        }
        else {
            argv[1] = Nan::Null();
        }
    }

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
