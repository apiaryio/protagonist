#include "protagonist.h"

using namespace v8;
using namespace protagonist;

Result::Result()
{
}

Result::~Result()
{
}

Persistent<Function> Result::constructor;

void Result::Init(Handle<Object> exports)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(String::NewSymbol("Result"));

    constructor = Persistent<Function>::New(t->GetFunction());
    exports->Set(String::NewSymbol("Result"), constructor);
}

Handle<Value> Result::New(const Arguments& args)
{
    HandleScope scope;
    Result* result = ::new Result();
    result->Wrap(args.This());
    return scope.Close(args.This());
}

Handle<Value> Result::NewInstance()
{
    HandleScope scope;
    Local<Object> instance = constructor->NewInstance();
    return scope.Close(instance);
}

v8::Local<v8::Object> Result::WrapResult(const snowcrash::Report& report,
                                         const snowcrash::Blueprint& blueprint,
                                         const snowcrash::SourceMap<snowcrash::Blueprint>& sourcemap,
                                         const snowcrash::BlueprintParserOptions& options)
{
    Local<Object> resultWrap = constructor->NewInstance();

    static const char* AstKey = "ast";
    static const char* WarningsKey = "warnings";
    static const char* SourcemapKey = "sourcemap";

    if (report.error.code == snowcrash::Error::OK)
        resultWrap->Set(String::NewSymbol(AstKey), Blueprint::WrapBlueprint(blueprint));
    else 
        resultWrap->Set(String::NewSymbol(AstKey), Local<Value>::New(Null()));

    Local<Object> warnings = Array::New(report.warnings.size());
    size_t i = 0;

    for (snowcrash::Warnings::const_iterator it = report.warnings.begin();
         it != report.warnings.end();
         ++it, ++i) {

        warnings->Set(i, SourceAnnotation::WrapSourceAnnotation(*it));
    }

    resultWrap->Set(String::NewSymbol(WarningsKey), warnings);

    // Set source map only if requested
    if (report.error.code == snowcrash::Error::OK && (options & snowcrash::ExportSourcemapOption) != 0)
        resultWrap->Set(String::NewSymbol(SourcemapKey), Sourcemap::WrapBlueprint(sourcemap));
    else
        resultWrap->Set(String::NewSymbol(SourcemapKey), Local<Value>::New(Null()));

    return resultWrap;
}
