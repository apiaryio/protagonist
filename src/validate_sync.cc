#include <string>
#include <sstream>
#include "v8_wrapper.h"
#include "protagonist.h"
#include "drafter_private.h"
#include "snowcrash.h"
#include "RefractDataStructure.h"
#include "ConversionContext.h"

using std::string;
using namespace v8;
using namespace protagonist;

NAN_METHOD(protagonist::ValidateSync) {
    Nan::HandleScope scope;

    // Check arguments
    if (info.Length() != 1 && info.Length() != 2) {
        Nan::ThrowTypeError("wrong number of arguments, `validateSync(string, options)` expected");
        return;
    }

    if (!info[0]->IsString()) {
        Nan::ThrowTypeError("wrong argument - string expected, `validateSync(string, options)`");
        return;
    }

    if (info.Length() == 2 && !info[1]->IsObject()) {
        Nan::ThrowTypeError("wrong argument - object expected, `validateSync(string, options)`");
        return;
    }

    // Get source data
    String::Utf8Value sourceData(info[0]->ToString());

    // Prepare options
    snowcrash::BlueprintParserOptions options = 0;

    if (info.Length() == 2) {
        OptionsResult *optionsResult = ParseOptionsObject(Handle<Object>::Cast(info[1]), true);

        if (optionsResult->error != NULL) {
            Nan::ThrowTypeError(optionsResult->error);
            return;
        }

        options = optionsResult->options;
        free(optionsResult);
    }

    sos::Object result = Validate::Do(*sourceData, options);

    if (result.empty()) {
        info.GetReturnValue().Set(Nan::Null());
    }
    else {
        info.GetReturnValue().Set(v8_wrap(result)->ToObject());
    }
}
