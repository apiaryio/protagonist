#include "protagonist.h"
#include "snowcrash.h"
#include "drafter_private.h"
#include "RefractDataStructure.h"
#include "ConversionContext.h"

using namespace v8;
using namespace protagonist;

Validate::Validate()
{
}

Validate::~Validate()
{
}

Nan::Persistent<Function> Validate::constructor;

void Validate::Init(Handle<Object> exports)
{
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->SetClassName(Nan::New<String>("Validate").ToLocalChecked());
    t->InstanceTemplate()->SetInternalFieldCount(1);

    constructor.Reset(t->GetFunction());
    exports->Set(Nan::New<String>("Validate").ToLocalChecked(), t->GetFunction());
}

NAN_METHOD(Validate::New)
{
    Nan::HandleScope scope;

    Validate* validate = ::new Validate();
    validate->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

sos::Object Validate::Do(const mdp::ByteBuffer& sourceData,
                         const snowcrash::BlueprintParserOptions& options)
{
    // Prepare Options
    drafter_parse_options drafterOptions = {
        static_cast<bool>(options & snowcrash::RequireBlueprintNameOption)
    };

    // Validate the source data
    drafter_result* parseResult = drafter_check_blueprint_with_options(sourceData.c_str(), drafterOptions);

    // Sample context
    drafter::WrapperOptions wrapperOptions(drafter::RefractASTType);
    drafter::ConversionContext context(wrapperOptions);

    sos::Object result = SerializeRefract(parseResult, context);

    drafter_free_result(parseResult);

    return result;
}
