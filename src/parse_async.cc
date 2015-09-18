#include <string>
#include <sstream>
#include "protagonist.h"
#include "snowcrash.h"
#include "drafter.h"

using std::string;
using namespace v8;
using namespace protagonist;

// Async Parse
void AsyncParse(uv_work_t* request);

// Async Parse Handler
void AsyncParseAfter(uv_work_t* request);

// Threadpooling libuv baton
struct Baton {

    // Callback
    Nan::Persistent<Function> callback;

    // Input
    snowcrash::BlueprintParserOptions options;
    drafter::ASTType astType;
    mdp::ByteBuffer sourceData;

    // Output
    snowcrash::ParseResult<snowcrash::Blueprint> parseResult;
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

    // Get source data
    String::Utf8Value sourceData(info[0]->ToString());

    // Prepare options
    snowcrash::BlueprintParserOptions options = 0;
    drafter::ASTType astType = drafter::RefractASTType;

    if (info.Length() == 3) {
        OptionsResult *optionsResult = ParseOptionsObject(Handle<Object>::Cast(info[1]));

        if (optionsResult->error != NULL) {
            Nan::ThrowTypeError(optionsResult->error);
            return;
        }

        options = optionsResult->options;
        astType = optionsResult->astType;
        free(optionsResult);
    }

    // Get Callback
    Local<Function> callback = (info.Length() == 3) ?  Local<Function>::Cast(info[2]) : Local<Function>::Cast(info[1]);

    // Prepare threadpool baton
    Baton* baton = ::new Baton();
    baton->options = options;
    baton->astType = astType;
    baton->sourceData = *sourceData;
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
    Baton* baton = static_cast<Baton*>(request->data);

    snowcrash::ParseResult<snowcrash::Blueprint> parseResult;

    // Parse the source data
    drafter::ParseBlueprint(baton->sourceData, baton->options, parseResult);

    baton->parseResult = parseResult;
}

void AsyncParseAfter(uv_work_t* request) {
    Nan::HandleScope scope;
    Baton* baton = static_cast<Baton*>(request->data);

    // Prepare report
    const unsigned argc = 2;
    Local<Value> argv[argc];

    argv[1] = Result::WrapResult(baton->parseResult, baton->options, baton->astType);

    // Error Object
    if (baton->parseResult.report.error.code == snowcrash::Error::OK)
        argv[0] = Nan::Null();
    else
        argv[0] = SourceAnnotation::WrapSourceAnnotation(baton->parseResult.report.error);

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
