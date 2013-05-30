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
            else {
                // Unrecognized option
                std::stringstream ss;
                ss << "unrecognized option '" << *String::Utf8Value(key) << "', expected: ";
                ss << "'" << RequireBlueprintNameOptionKey << "'";
                ThrowException(Exception::TypeError(String::New(ss.str().c_str())));
                return scope.Close(Undefined());
            }
        } 
    }

    // Parse the source data
    snowcrash::Result result;
    snowcrash::Blueprint ast;
    snowcrash::parse(*sourceData, options, result, ast);

    // Prepare result
    const unsigned argc = 2;
    Local<Value> argv[argc];

    // Error Object
    if (result.error.code == snowcrash::Error::OK)
        argv[0] = Local<Value>::New(Null());

    else
        argv[0] = SourceAnnotation::WrapSourceAnnotation(result.error);


    argv[1] = Result::WrapResult(result, ast);    

    // Callback
    Local<Function> callback = (args.Length() == 3) ?  Local<Function>::Cast(args[2]) : Local<Function>::Cast(args[1]);
    callback->Call(Context::GetCurrent()->Global(), argc, argv);

    return scope.Close(Undefined());
}    
