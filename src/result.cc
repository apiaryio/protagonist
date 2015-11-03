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

Nan::Persistent<Function> Result::constructor;

void Result::Init(Handle<Object> exports)
{
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->SetClassName(Nan::New<String>("Result").ToLocalChecked());
    t->InstanceTemplate()->SetInternalFieldCount(1);

    constructor.Reset(t->GetFunction());
    exports->Set(Nan::New<String>("Result").ToLocalChecked(), t->GetFunction());
}

NAN_METHOD(Result::New)
{
    Nan::HandleScope scope;

    Result* result = ::new Result();
    result->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
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
        result = drafter::WrapResult(parseResult, drafter::WrapperOptions(astType, options & snowcrash::ExportSourcemapOption));
    }
    catch (snowcrash::Error& error) {
        parseResult.report.error = error;
    }

    if (astType == drafter::NormalASTType && parseResult.report.error.code != snowcrash::Error::OK) {
        result.set(AstKey, sos::Null());

        if ((options & snowcrash::ExportSourcemapOption) != 0) {
            result.set(SourcemapKey, sos::Null());
        }
    }

    result.unset(ErrorKey);

    return v8_wrap(result)->ToObject();
}
