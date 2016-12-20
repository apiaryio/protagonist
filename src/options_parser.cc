#include "protagonist.h"
#include "snowcrash.h"

using namespace v8;
using namespace protagonist;

//static const std::string RenderDescriptionsOptionKey = "renderDescriptions";
static const std::string RequireBlueprintNameOptionKey = "requireBlueprintName";
static const std::string ExportSourcemapOptionKey = "exportSourcemap";
static const std::string GenerateSourceMapOptionKey = "generateSourceMap";

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
            // RequireBlueprintNameOption
            if (value->IsTrue())
                optionsResult->options |= snowcrash::RequireBlueprintNameOption;
            else
                optionsResult->options &= snowcrash::RequireBlueprintNameOption;
        }
        else if (!forValidate) {
            if (ExportSourcemapOptionKey == *strKey || GenerateSourceMapOptionKey == *strKey) {
                // ExportSourcemapOption
                if (value->IsTrue())
                    optionsResult->options |= snowcrash::ExportSourcemapOption;
                else
                    optionsResult->options &= snowcrash::ExportSourcemapOption;
            }
            else {
                // Unrecognized option
                std::stringstream ss;
                ss << "unrecognized option '" << *strKey << "', expected: ";
                ss << "'" << RequireBlueprintNameOptionKey << "', '";
                ss << GenerateSourceMapOptionKey << "'";

                optionsResult->error = ss.str().c_str();
                return optionsResult;
            }
        }
        else {
            // Unrecognize option
            std::stringstream ss;
            ss << "unrecognized option '" << *strKey << "', expected: ";
            ss << "'" << RequireBlueprintNameOptionKey << '"';

            optionsResult->error = ss.str().c_str();
            return optionsResult;
        }
    }

    return optionsResult;
}
