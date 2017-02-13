#include <sstream>
#include "protagonist.h"

using namespace v8;
using namespace protagonist;

//static const std::string RenderDescriptionsOptionKey = "renderDescriptions";
static const std::string RequireBlueprintNameOptionKey = "requireBlueprintName";
static const std::string ExportSourcemapOptionKey = "exportSourcemap";
static const std::string GenerateSourceMapOptionKey = "generateSourceMap";

static char* AllocErrorMessageForUnrecognisedOption(const String::Utf8Value& key, const bool forValidate) {

    std::stringstream ss;
    ss << "unrecognized option '" << *key << "', expected: ";
    ss << "'" << RequireBlueprintNameOptionKey << "'";

    if (!forValidate) {
        ss << ", '" << GenerateSourceMapOptionKey << "'";
    }

    return strdup(ss.str().c_str());
}

OptionsResult* protagonist::ParseOptionsObject(Handle<Object> optionsObject, bool forValidate) {
    OptionsResult *optionsResult = (OptionsResult *) calloc(1, sizeof(OptionsResult));
    optionsResult->serializeOptions.format = DRAFTER_SERIALIZE_JSON;

    const Local<Array> properties = optionsObject->GetPropertyNames();
    const uint32_t length = properties->Length();

    for (uint32_t i = 0 ; i < length ; ++i) {
        const Local<Value> key = properties->Get(i);
        const Local<Value> value = optionsObject->Get(key);

        const String::Utf8Value strKey(key);

        if (RequireBlueprintNameOptionKey == *strKey) {
            if (value->IsTrue()) {
                optionsResult->parseOptions.requireBlueprintName = true;
            }
        }
        else if (!forValidate) {
            if (ExportSourcemapOptionKey == *strKey || GenerateSourceMapOptionKey == *strKey) {
                if (value->IsTrue()) {
                    optionsResult->serializeOptions.sourcemap = true;
                }
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

