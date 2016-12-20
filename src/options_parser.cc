#include "protagonist.h"
#include "snowcrash.h"

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

static void SetOption(snowcrash::BlueprintParserOptions& options, const Local<Value> value, const snowcrash::BlueprintParserOption flag) {

    if (value->IsTrue()) {
        options |= flag;
    }
    else {
        options &= flag;
    }
}

OptionsResult* protagonist::ParseOptionsObject(Handle<Object> optionsObject, bool forValidate) {
    OptionsResult *optionsResult = (OptionsResult *) malloc(sizeof(OptionsResult));

    optionsResult->options = 0;
    optionsResult->astType = drafter::RefractASTType;
    optionsResult->error = NULL;

    const Local<Array> properties = optionsObject->GetPropertyNames();
    const uint32_t length = properties->Length();

    for (uint32_t i = 0 ; i < length ; ++i) {
        const Local<Value> key = properties->Get(i);
        const Local<Value> value = optionsObject->Get(key);

        const String::Utf8Value strKey(key);

        if (RequireBlueprintNameOptionKey == *strKey) {
            SetOption(optionsResult->options, value, snowcrash::RequireBlueprintNameOption);
        }
        else if (!forValidate) {
            if (ExportSourcemapOptionKey == *strKey || GenerateSourceMapOptionKey == *strKey) {
                SetOption(optionsResult->options, value, snowcrash::ExportSourcemapOption);
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

