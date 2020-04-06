#include <sstream>

#include "protagonist.h"
#include "options.h"

using namespace v8;
using namespace protagonist;

//static const std::string RenderDescriptionsOptionKey = "renderDescriptions";
static const std::string RequireBlueprintNameOptionKey = "requireBlueprintName";
static const std::string ExportSourcemapOptionKey = "exportSourcemap";
static const std::string GenerateSourceMapOptionKey = "generateSourceMap";
static const std::string GenerateMessageBodyOptionKey = "generateMessageBody";
static const std::string GenerateMessageBodySchemaOptionKey = "generateMessageBodySchema";

static ErrorMessage ErrorMessageForUnrecognisedOption(const Nan::Utf8String& key, const bool forValidate)
{
    std::stringstream ss;
    ss << "unrecognized option '" << *key << "', expected: ";
    ss << "'" << RequireBlueprintNameOptionKey << "'";
    ss << ", '" << GenerateMessageBodyOptionKey<< "'";
    ss << ", '" << GenerateMessageBodySchemaOptionKey<< "'";

    if (!forValidate) {
        ss << ", '" << GenerateSourceMapOptionKey << "'";
    }

    return ss.str();
}

ErrorMessage protagonist::ParseOptionsObject(Options& options, Local<Object> optionsObject, bool forValidate) {
    Local<Context> context = Nan::GetCurrentContext();
    Isolate *isolate = context->GetIsolate();

    const Local<Array> properties = optionsObject->GetPropertyNames(context).ToLocalChecked();
    const uint32_t length = properties->Length();

    for (uint32_t i = 0 ; i < length ; ++i) {
        Local<Value> key = properties->Get(context, i).ToLocalChecked();
        const Nan::Utf8String strKey(key);

        v8::MaybeLocal<v8::Value> maybeValue = optionsObject->Get(context, key);

        // all options are boolean w/ false value default
        const Local<Value> value = maybeValue.FromMaybe(Local<Value>(False(isolate)));

        if (RequireBlueprintNameOptionKey == *strKey) {
            if (value->IsTrue()) options.setNameRequired();
        } else if (GenerateMessageBodyOptionKey == *strKey) {
            if (value->IsFalse()) options.setSkipGenBodies();
        } else if (GenerateMessageBodySchemaOptionKey == *strKey) {
            if (value->IsFalse()) options.setSkipGenBodySchemas();
        } else if (!forValidate) {
            if (ExportSourcemapOptionKey == *strKey ||
                GenerateSourceMapOptionKey == *strKey) {
                if (value->IsTrue()) options.setSerializeSourcemaps();
            } else {
                return ErrorMessageForUnrecognisedOption(strKey, forValidate);
            }
        } else {
            return ErrorMessageForUnrecognisedOption(strKey, forValidate);
        }
    }
    return {};
}
