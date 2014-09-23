#include <node.h>
#include <v8.h>
#include <string>
#include <sstream>
#include "protagonist.h"
#include "snowcrash.h"

using std::string;
using namespace v8;
using namespace protagonist;

//static const std::string RenderDescriptionsOptionKey = "renderDescriptions";
static const std::string RequireBlueprintNameOptionKey = "requireBlueprintName";
static const std::string ExportSourcemapOptionKey = "exportSourcemap";

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
    mdp::ByteBuffer sourceData;

    // Output
    snowcrash::Report report;
    snowcrash::Blueprint ast;
    snowcrash::SourceMap<snowcrash::Blueprint> sourcemap;
};

Handle<Value> protagonist::Parse(const Arguments& args) {
    HandleScope scope;

    // Check arguments
    if (args.Length() != 2 && args.Length() != 3) {
        ThrowException(Exception::TypeError(String::New("wrong number of arguments, `parse(string, options, callback)` expected")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsString()) {
        ThrowException(Exception::TypeError(String::New("wrong argument - string expected, `parse(string, options, callback)`")));
        return scope.Close(Undefined());
    }

    if ((args.Length() == 2 && !args[1]->IsFunction()) ||
        (args.Length() == 3 && !args[2]->IsFunction())) {
        ThrowException(Exception::TypeError(String::New("wrong argument - callback expected, `parse(string, options, callback)`")));
        return scope.Close(Undefined());
    }

    if (args.Length() == 3 && !args[1]->IsObject()) {
        ThrowException(Exception::TypeError(String::New("wrong argument - object expected, `parse(string, options, callback)`")));
        return scope.Close(Undefined());
    }    
    
    // Get source data
    String::Utf8Value sourceData(args[0]->ToString());

    // Prepare options
    snowcrash::BlueprintParserOptions options = 0;

    if (args.Length() == 3) {
        Handle<Object> optionsObject = Handle<Object>::Cast(args[1]);
        const Local<Array> properties = optionsObject->GetPropertyNames();
        const uint32_t length = properties->Length();
        
        for (uint32_t i = 0 ; i < length ; ++i) {
            const Local<Value> key = properties->Get(i);
            const Local<Value> value = optionsObject->Get(key);

            if (RequireBlueprintNameOptionKey == *String::Utf8Value(key)) {
                // RequireBlueprintNameOption
                if (value->IsTrue())
                    options |= snowcrash::RequireBlueprintNameOption;
                else
                    options &= snowcrash::RequireBlueprintNameOption;
            }
            else if (ExportSourcemapOptionKey == *String::Utf8Value(key)) {
                // ExportSourcemapOption
                if (value->IsTrue())
                    options |= snowcrash::ExportSourcemapOption;
                else
                    options &= snowcrash::ExportSourcemapOption;
            }
            else {
                // Unrecognized option
                std::stringstream ss;
                ss << "unrecognized option '" << *String::Utf8Value(key) << "', expected: ";
                ss << "'" << RequireBlueprintNameOptionKey << "' or '" << ExportSourcemapOptionKey << "'";
                ThrowException(Exception::TypeError(String::New(ss.str().c_str())));
                return scope.Close(Undefined());
            }
        } 
    }

    // Get Callback
    Local<Function> callback = (args.Length() == 3) ?  Local<Function>::Cast(args[2]) : Local<Function>::Cast(args[1]);

    // Prepare threadpool baton
    Baton* baton = ::new Baton();
    baton->options = options;
    baton->sourceData = *sourceData;
    baton->callback = Persistent<Function>::New(callback);

    // This creates the work request struct.
    uv_work_t *request = ::new uv_work_t();
    request->data = baton;

    // Schedule the work request
    int status = uv_queue_work(uv_default_loop(), 
                                request, 
                                AsyncParse,
                                (uv_after_work_cb)AsyncParseAfter);
    assert(status == 0);
    return scope.Close(Undefined());
}    

void AsyncParse(uv_work_t* request) {
    Baton* baton = static_cast<Baton*>(request->data);

    snowcrash::ParseResult<snowcrash::Blueprint> parseResult;

    // Parse the source data
    snowcrash::parse(baton->sourceData, baton->options, parseResult);

    baton->report = parseResult.report;
    baton->ast = parseResult.node;
    baton->sourcemap = parseResult.sourceMap;
}

void AsyncParseAfter(uv_work_t* request) {
    HandleScope scope;
    Baton* baton = static_cast<Baton*>(request->data);

    // Prepare report
    const unsigned argc = 2;
    Local<Value> argv[argc];

    // Error Object
    if (baton->report.error.code == snowcrash::Error::OK)
        argv[0] = Local<Value>::New(Null());
    else
        argv[0] = SourceAnnotation::WrapSourceAnnotation(baton->report.error);

    argv[1] = Result::WrapResult(baton->report, baton->ast, baton->sourcemap, baton->options);

    TryCatch try_catch;
    baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);

    if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
    }

    baton->callback.Dispose();
    delete baton;
    delete request;
}
