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
    Persistent<Function> callback;

    // Input
    snowcrash::BlueprintParserOptions options;
    drafter::ASTType astType;
    mdp::ByteBuffer sourceData;

    // Output
    snowcrash::ParseResult<snowcrash::Blueprint> parseResult;
};

NAN_METHOD(protagonist::Parse) {
    NanScope();

    // Check arguments
    if (args.Length() != 2 && args.Length() != 3) {
        NanThrowTypeError("wrong number of arguments, `parse(string, options, callback)` expected");
        NanReturnUndefined();
    }

    if (!args[0]->IsString()) {
        NanThrowTypeError("wrong argument - string expected, `parse(string, options, callback)`");
        NanReturnUndefined();
    }

    if ((args.Length() == 2 && !args[1]->IsFunction()) ||
        (args.Length() == 3 && !args[2]->IsFunction())) {

        NanThrowTypeError("wrong argument - callback expected, `parse(string, options, callback)`");
        NanReturnUndefined();
    }

    if (args.Length() == 3 && !args[1]->IsObject()) {
        NanThrowTypeError("wrong argument - object expected, `parse(string, options, callback)`");
        NanReturnUndefined();
    }

    // Get source data
    String::Utf8Value sourceData(args[0]->ToString());

    // Prepare options
    snowcrash::BlueprintParserOptions options = 0;
    drafter::ASTType astType = drafter::RefractASTType;

    if (args.Length() == 3) {
        OptionsResult *optionsResult = ParseOptionsObject(Handle<Object>::Cast(args[1]));

        if (optionsResult->error != NULL) {
            NanThrowTypeError(optionsResult->error);
            NanReturnUndefined();
        }

        options = optionsResult->options;
        astType = optionsResult->astType;
        free(optionsResult);
    }

    // Get Callback
    Local<Function> callback = (args.Length() == 3) ?  Local<Function>::Cast(args[2]) : Local<Function>::Cast(args[1]);

    // Prepare threadpool baton
    Baton* baton = ::new Baton();
    baton->options = options;
    baton->astType = astType;
    baton->sourceData = *sourceData;
    NanAssignPersistent<Function>(baton->callback, callback);

    // This creates the work request struct.
    uv_work_t *request = ::new uv_work_t();
    request->data = baton;

    // Schedule the work request
    int status = uv_queue_work(uv_default_loop(),
                                request,
                                AsyncParse,
                                (uv_after_work_cb)AsyncParseAfter);

    assert(status == 0);
    NanReturnUndefined();
}

void AsyncParse(uv_work_t* request) {
    Baton* baton = static_cast<Baton*>(request->data);

    snowcrash::ParseResult<snowcrash::Blueprint> parseResult;

    // Parse the source data
    drafter::ParseBlueprint(baton->sourceData, baton->options, parseResult);

    baton->parseResult = parseResult;
}

void AsyncParseAfter(uv_work_t* request) {
    NanScope();
    Baton* baton = static_cast<Baton*>(request->data);

    // Prepare report
    const unsigned argc = 2;
    Local<Value> argv[argc];

    // Error Object
    if (baton->parseResult.report.error.code == snowcrash::Error::OK)
        argv[0] = NanNull();
    else
        argv[0] = SourceAnnotation::WrapSourceAnnotation(baton->parseResult.report.error);

    argv[1] = Result::WrapResult(baton->parseResult, baton->options, baton->astType);

    TryCatch try_catch;
    Local<Function> callback = NanNew<Function>(baton->callback);
    callback->Call(NanGetCurrentContext()->Global(), argc, argv);

    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }

    NanDisposePersistent(baton->callback);
    delete baton;
    delete request;
}
