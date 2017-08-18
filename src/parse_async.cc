#include <string>
#include "protagonist.h"
#include "drafter.h"
#include "refractToV8.h"

using std::string;
using namespace v8;
using namespace protagonist;

// Async Parse
void AsyncParse(uv_work_t* request);

// Async Parse Handler
void AsyncParseAfter(uv_work_t* request);

// Threadpooling libuv baton
struct ParseBaton {

    ~ParseBaton(){
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
    drafter_serialize_options serializeOptions;
    Nan::Utf8String *sourceData;

    // Output
    drafter_result *result;
    int parse_err_code;
};

NAN_METHOD(protagonist::Parse) {
    Nan::HandleScope scope;

    // Check arguments
    if (info.Length() != 2 && info.Length() != 3) {
        Nan::ThrowTypeError("wrong number of arguments, `parse(string, options, callback)` expected");
        return;
    }

    if (!info[0]->IsString()) {
        Nan::ThrowTypeError("wrong argument - string expected, `parse(string, options, callback)`");
        return;
    }

    if ((info.Length() == 2 && !info[1]->IsFunction()) ||
        (info.Length() == 3 && !info[2]->IsFunction())) {

        Nan::ThrowTypeError("wrong argument - callback expected, `parse(string, options, callback)`");
        return;
    }

    if (info.Length() == 3 && !info[1]->IsObject()) {
        Nan::ThrowTypeError("wrong argument - object expected, `parse(string, options, callback)`");
        return;
    }

    // Prepare options
    drafter_parse_options parseOptions = {false};
    drafter_serialize_options serializeOptions = {false, DRAFTER_SERIALIZE_JSON};

    if (info.Length() == 3) {
        OptionsResult *optionsResult = ParseOptionsObject(Handle<Object>::Cast(info[1]), false);

        if (optionsResult->error != NULL) {
            Nan::ThrowTypeError(optionsResult->error);
        }

        parseOptions = optionsResult->parseOptions;
        serializeOptions = optionsResult->serializeOptions;
        FreeOptionsResult(&optionsResult);
    }

    // Get Callback
    Local<Function> callback = (info.Length() == 3) ? Local<Function>::Cast(info[2]) : Local<Function>::Cast(info[1]);

    // Prepare threadpool baton
    ParseBaton* baton = ::new ParseBaton();
    baton->parseOptions = parseOptions;
    baton->serializeOptions = serializeOptions;
    baton->sourceData = new Nan::Utf8String(info[0]);
    baton->callback.Reset(callback);

    // This creates the work request struct.
    uv_work_t *request = ::new uv_work_t();
    request->data = baton;

    // Schedule the work request
    int status = uv_queue_work(uv_default_loop(),
                                request,
                                AsyncParse,
                                (uv_after_work_cb)AsyncParseAfter);

    assert(status == 0);
    return;
}

void AsyncParse(uv_work_t* request) {
    ParseBaton* baton = static_cast<ParseBaton*>(request->data);

    // Parse the source data
    baton->parse_err_code = drafter_parse_blueprint(*(*baton->sourceData),
                                                    &baton->result,
                                                    baton->parseOptions);
}

void AsyncParseAfter(uv_work_t* request) {
    Nan::HandleScope scope;
    ParseBaton* baton = static_cast<ParseBaton*>(request->data);

    // Prepare report
    const unsigned argc = 2;
    Local<Value> argv[argc];

    argv[1] = refract2v8(baton->result, baton->serializeOptions);

    if (0 == baton->parse_err_code) {
        argv[0] = Nan::Null();
    }
    else {
        argv[0] = annotations2v8(baton->result);
    }

    TryCatch try_catch(v8::Isolate::GetCurrent());
    Local<Function> callback = Nan::New<Function>(baton->callback);
    callback->Call(Nan::GetCurrentContext()->Global(), argc, argv);

    if (try_catch.HasCaught()) {
        node::FatalException(v8::Isolate::GetCurrent(), try_catch);
    }

    baton->callback.Reset();
    delete baton;
    delete request;
}
