#include <string>
#include <sstream>
#include "protagonist.h"
#include "snowcrash.h"
#include "drafter.h"

using std::string;
using namespace v8;
using namespace protagonist;

NAN_METHOD(protagonist::ParseSync) {
    NanScope();

    // Check arguments
    if (args.Length() != 1 && args.Length() != 2) {
        NanThrowTypeError("wrong number of arguments, `parseSync(string, options)` expected");
        NanReturnUndefined();
    }

    if (!args[0]->IsString()) {
        NanThrowTypeError("wrong argument - string expected, `parseSync(string, options)`");
        NanReturnUndefined();
    }

    if (args.Length() == 2 && !args[1]->IsObject()) {
        NanThrowTypeError("wrong argument - object expected, `parseSync(string, options)`");
        NanReturnUndefined();
    }

    // Get source data
    String::Utf8Value sourceData(args[0]->ToString());

    // Prepare options
    snowcrash::BlueprintParserOptions options = 0;
    drafter::ASTType astType = drafter::RefractASTType;

    if (args.Length() == 2) {
        OptionsResult *optionsResult = ParseOptionsObject(Handle<Object>::Cast(args[1]));

        if (optionsResult->error != NULL) {
            NanThrowTypeError(optionsResult->error);
            NanReturnUndefined();
        }

        options = optionsResult->options;
        astType = optionsResult->astType;
        free(optionsResult);
    }

    // Parse the source data
    snowcrash::ParseResult<snowcrash::Blueprint> parseResult;
    drafter::ParseBlueprint(*sourceData, options, parseResult);

    if (parseResult.report.error.code != snowcrash::Error::OK) {
        NanThrowError(SourceAnnotation::WrapSourceAnnotation(parseResult.report.error));
        NanReturnUndefined();
    }

    NanReturnValue(Result::WrapResult(parseResult, options, astType));
}
