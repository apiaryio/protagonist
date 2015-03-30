#include "protagonist.h"
#include "SerializeAST.h"
#include "SerializeSourcemap.h"
#include "v8_wrapper.h"

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

v8::Local<v8::Object> Result::WrapResult(const snowcrash::Report& report,
                                         const snowcrash::Blueprint& blueprint,
                                         const snowcrash::SourceMap<snowcrash::Blueprint>& sourcemap,
                                         const snowcrash::BlueprintParserOptions& options)
{
    Local<Function> cons = NanNew<Function>(constructor);
    Local<Object> resultWrap = cons->NewInstance();

    static const char* AstKey = "ast";
    static const char* WarningsKey = "warnings";
    static const char* SourcemapKey = "sourcemap";

    if (report.error.code == snowcrash::Error::OK) {
        sos::Object blueprintSerializationWrap = drafter::WrapBlueprint(blueprint);

        resultWrap->Set(NanNew<String>(AstKey), v8_wrap(blueprintSerializationWrap));
    }
    else
        resultWrap->Set(NanNew<String>(AstKey), NanNull());

    Local<Object> warnings = NanNew<Array>(report.warnings.size());
    size_t i = 0;

    for (snowcrash::Warnings::const_iterator it = report.warnings.begin();
         it != report.warnings.end();
         ++it, ++i) {

        warnings->Set(i, SourceAnnotation::WrapSourceAnnotation(*it));
    }

    resultWrap->Set(NanNew<String>(WarningsKey), warnings);

    // Set source map only if requested
    if (report.error.code == snowcrash::Error::OK && (options & snowcrash::ExportSourcemapOption) != 0) {
        sos::Object sourcemapSerializationWrap = drafter::WrapBlueprintSourcemap(sourcemap);

        resultWrap->Set(NanNew<String>(SourcemapKey), v8_wrap(sourcemapSerializationWrap));
    }
    else
        resultWrap->Set(NanNew<String>(SourcemapKey), NanNull());

    return resultWrap;
}
