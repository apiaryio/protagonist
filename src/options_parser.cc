#include <sstream>
#include "protagonist.h"

using namespace v8;
using namespace protagonist;

//static const std::string RenderDescriptionsOptionKey = "renderDescriptions";
static const std::string RequireBlueprintNameOptionKey = "requireBlueprintName";
static const std::string ExportSourcemapOptionKey = "exportSourcemap";
static const std::string GenerateSourceMapOptionKey = "generateSourceMap";

static char* AllocErrorMessageForUnrecognisedOption(const Nan::Utf8String& key, const bool forValidate) {

    std::stringstream ss;
    ss << "unrecognized option '" << *key << "', expected: ";
    ss << "'" << RequireBlueprintNameOptionKey << "'";

    if (!forValidate) {
        ss << ", '" << GenerateSourceMapOptionKey << "'";
    }

    return strdup(ss.str().c_str());
}

OptionsResult* protagonist::ParseOptionsObject(Local<Object> optionsObject, bool forValidate) {
    Local<Context> context = Nan::GetCurrentContext();
    Isolate *isolate = context->GetIsolate();
    OptionsResult *optionsResult = (OptionsResult *) malloc(sizeof(OptionsResult));

    optionsResult->serializeOptions.format = DRAFTER_SERIALIZE_JSON;
    optionsResult->serializeOptions.sourcemap = false;
    optionsResult->parseOptions.requireBlueprintName = false;
    optionsResult->error = NULL;

    const Local<Array> properties = optionsObject->GetPropertyNames(context).ToLocalChecked();
    const uint32_t length = properties->Length();

    for (uint32_t i = 0 ; i < length ; ++i) {
        Local<Value> key = properties->Get(context, i).ToLocalChecked();
        const Nan::Utf8String strKey(key);

        v8::MaybeLocal<v8::Value> maybeValue = optionsObject->Get(context, key);

        // all options are boolean w/ false value default
        const Local<Value> value = maybeValue.FromMaybe(Local<Value>(False(isolate)));

        if (RequireBlueprintNameOptionKey == *strKey) {
            optionsResult->parseOptions.requireBlueprintName = value->IsTrue();
        }
        else if (!forValidate) {
            if (ExportSourcemapOptionKey == *strKey || GenerateSourceMapOptionKey == *strKey) {
                optionsResult->serializeOptions.sourcemap = value->IsTrue();
            }
            else {
                optionsResult->error = AllocErrorMessageForUnrecognisedOption(strKey, forValidate);
                return optionsResult;
            }
        }
        else {
            optionsResult->error = AllocErrorMessageForUnrecognisedOption(strKey, forValidate);
            return optionsResult;
        }
    }

    return optionsResult;
}

void protagonist::FreeOptionsResult(OptionsResult** optionsResult) {

    if (*optionsResult != NULL) {
        if ((*optionsResult)->error != NULL) {
            free((*optionsResult)->error);
        }
        free(*optionsResult);
    }
    *optionsResult = NULL;

}
