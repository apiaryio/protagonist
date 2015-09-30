#include "protagonist.h"
#include "SerializeResult.h"
#include "v8_wrapper.h"
#include "snowcrash.h"

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
    NanScope();

    Local<FunctionTemplate> t = NanNew<FunctionTemplate>(New);
    t->SetClassName(NanNew<String>("Result"));
    t->InstanceTemplate()->SetInternalFieldCount(1);

    NanAssignPersistent<Function>(constructor, t->GetFunction());
    exports->Set(NanNew<String>("Result"), t->GetFunction());
}

NAN_METHOD(Result::New)
{
    NanScope();

    Result* result = ::new Result();
    result->Wrap(args.This());

    NanReturnValue(args.This());
}

v8::Local<v8::Object> Result::WrapResult(snowcrash::ParseResult<snowcrash::Blueprint>& parseResult,
                                         const snowcrash::BlueprintParserOptions& options,
                                         const drafter::ASTType& astType)
{
    static const char* AstKey = "ast";
    static const char* ErrorKey = "error";
    static const char* SourcemapKey = "sourcemap";

    sos::Object result;

    try {
        result = drafter::WrapResult(parseResult, options, astType);
    }
    catch (snowcrash::Error& error) {
        parseResult.report.error = error;
    }

    if (parseResult.report.error.code != snowcrash::Error::OK) {
        result.set(AstKey, sos::Null());

        if ((options & snowcrash::ExportSourcemapOption) != 0) {
            result.set(SourcemapKey, sos::Null());
        }
    }
    else {
        result.unset(ErrorKey);
    }

    return v8_wrap(result)->ToObject();
}
