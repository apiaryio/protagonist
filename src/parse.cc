#include <node.h>
#include <v8.h>
#include <string>
#include "protagonist.h"
#include "snowcrash.h"

using std::string;
using namespace v8;
using namespace protagonist;

Handle<Value> protagonist::Parse(const Arguments& args) {
    HandleScope scope;

    // Check arguments
    if (args.Length() < 2) {
        ThrowException(Exception::TypeError(String::New("Wrong number of arguments, `parse(string, callback)` expected.")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsString()) {
        ThrowException(Exception::TypeError(String::New("Wrong argument, string expected.")));
        return scope.Close(Undefined());
    }

    if (!args[1]->IsFunction()) {
        ThrowException(Exception::TypeError(String::New("Wrong argument, function expected.")));
        return scope.Close(Undefined());
    }
    
    // Get source data
    v8::String::Utf8Value sourceData(args[0]->ToString());

    // Parse the source data
    snowcrash::Result result;
    snowcrash::Blueprint blueprintAst;
    snowcrash::parse(*sourceData, result, blueprintAst);

    // Prepare result
    const unsigned argc = 2;
    Local<Value> argv[argc];

    // Error Object
    argv[0] = Local<Value>::New(Null());

    // Blueprint AST
    argv[1] = Blueprint::WrapBlueprint(blueprintAst);;

    // Callback
    Local<Function> callback = Local<Function>::Cast(args[1]);
    callback->Call(Context::GetCurrent()->Global(), argc, argv);
    

    return scope.Close(Undefined());
}    
