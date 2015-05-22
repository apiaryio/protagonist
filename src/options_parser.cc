#include "protagonist.h"
#include "snowcrash.h"

using namespace v8;
using namespace protagonist;

//static const std::string RenderDescriptionsOptionKey = "renderDescriptions";
static const std::string RequireBlueprintNameOptionKey = "requireBlueprintName";
static const std::string ExportSourcemapOptionKey = "exportSourcemap";

OptionsResult* protagonist::ParseOptionsObject(Handle<Object> optionsObject) {
    OptionsResult *optionsResult = (OptionsResult *) malloc(sizeof(OptionsResult));

    optionsResult->options = 0;
    optionsResult->error = NULL;

    const Local<Array> properties = optionsObject->GetPropertyNames();
    const uint32_t length = properties->Length();

    for (uint32_t i = 0 ; i < length ; ++i) {
        const Local<Value> key = properties->Get(i);
        const Local<Value> value = optionsObject->Get(key);

        if (RequireBlueprintNameOptionKey == *String::Utf8Value(key)) {
            // RequireBlueprintNameOption
            if (value->IsTrue())
                optionsResult->options |= snowcrash::RequireBlueprintNameOption;
            else
                optionsResult->options &= snowcrash::RequireBlueprintNameOption;
        }
        else if (ExportSourcemapOptionKey == *String::Utf8Value(key)) {
            // ExportSourcemapOption
            if (value->IsTrue())
                optionsResult->options |= snowcrash::ExportSourcemapOption;
            else
                optionsResult->options &= snowcrash::ExportSourcemapOption;
        }
        else {
            // Unrecognized option
            std::stringstream ss;
            ss << "unrecognized option '" << *String::Utf8Value(key) << "', expected: ";
            ss << "'" << RequireBlueprintNameOptionKey << "' or '" << ExportSourcemapOptionKey << "'";

            optionsResult->error = ss.str().c_str();
            return optionsResult;
        }
    }

    return optionsResult;
}
