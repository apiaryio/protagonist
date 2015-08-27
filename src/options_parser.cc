#include "protagonist.h"
#include "snowcrash.h"

using namespace v8;
using namespace protagonist;

//static const std::string RenderDescriptionsOptionKey = "renderDescriptions";
static const std::string RequireBlueprintNameOptionKey = "requireBlueprintName";
static const std::string ExportSourcemapOptionKey = "exportSourcemap";
static const std::string TypeOptionKey = "type";

static const std::string TypeOptionAst = "ast";
static const std::string TypeOptionRefract = "refract";

OptionsResult* protagonist::ParseOptionsObject(Handle<Object> optionsObject) {
    OptionsResult *optionsResult = (OptionsResult *) malloc(sizeof(OptionsResult));

    optionsResult->options = 0;
    optionsResult->astType = drafter::RefractASTType;
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
        else if (TypeOptionKey == *String::Utf8Value(key)) {
            // TypeOption
            if (TypeOptionAst == *String::Utf8Value(value)) {
                optionsResult->astType = drafter::NormalASTType;
            } else if (TypeOptionRefract == *String::Utf8Value(value)) {
                optionsResult->astType = drafter::RefractASTType;
            } else {
                std::stringstream ss;
                ss << "unrecognized type '" << *String::Utf8Value(value) << "', expected '";
                ss << TypeOptionAst << "' or '" << TypeOptionRefract << "'";

                optionsResult->error = ss.str().c_str();
                return optionsResult;
            }
        }
        else {
            // Unrecognized option
            std::stringstream ss;
            ss << "unrecognized option '" << *String::Utf8Value(key) << "', expected: ";
            ss << "'" << RequireBlueprintNameOptionKey << "', '";
            ss << ExportSourcemapOptionKey << "' or '" << TypeOptionKey << '"';

            optionsResult->error = ss.str().c_str();
            return optionsResult;
        }
    }

    return optionsResult;
}
