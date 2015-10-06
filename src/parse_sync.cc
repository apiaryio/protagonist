#include <string>
#include <sstream>
#include "protagonist.h"
#include "snowcrash.h"
#include "drafter.h"

using std::string;
using namespace v8;
using namespace protagonist;

NAN_METHOD(protagonist::ParseSync) {
    Nan::HandleScope scope;

    // Check arguments
    if (info.Length() != 1 && info.Length() != 2) {
        Nan::ThrowTypeError("wrong number of arguments, `parseSync(string, options)` expected");
        return;
    }

    if (!info[0]->IsString()) {
        Nan::ThrowTypeError("wrong argument - string expected, `parseSync(string, options)`");
        return;
    }

    if (info.Length() == 2 && !info[1]->IsObject()) {
        Nan::ThrowTypeError("wrong argument - object expected, `parseSync(string, options)`");
        return;
    }

    // Get source data
    String::Utf8Value sourceData(info[0]->ToString());

    // Prepare options
    snowcrash::BlueprintParserOptions options = 0;
    drafter::ASTType astType = drafter::RefractASTType;

    if (info.Length() == 2) {
        OptionsResult *optionsResult = ParseOptionsObject(Handle<Object>::Cast(info[1]));

        if (optionsResult->error != NULL) {
            Nan::ThrowTypeError(optionsResult->error);
            return;
        }

        options = optionsResult->options;
        astType = optionsResult->astType;
        free(optionsResult);
    }

    // Parse the source data
    snowcrash::ParseResult<snowcrash::Blueprint> parseResult;
    drafter::ParseBlueprint(*sourceData, options, parseResult);

    Local<Object> result = Result::WrapResult(parseResult, options, astType);

    if (parseResult.report.error.code != snowcrash::Error::OK) {
        Nan::ThrowError((Local<Value>) SourceAnnotation::WrapSourceAnnotation(parseResult.report.error));
        return;
    }

    info.GetReturnValue().Set(result);
}
