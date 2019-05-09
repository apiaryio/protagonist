#include <string>
#include "protagonist.h"
#include "drafter.h"
#include "refractToV8.h"

using std::string;
using namespace v8;
using namespace protagonist;

NAN_METHOD(protagonist::ValidateSync) {
    Nan::HandleScope scope;
    Local<Context> context = Nan::GetCurrentContext();

    // Check arguments
    if (info.Length() != 1 && info.Length() != 2) {
        Nan::ThrowTypeError("wrong number of arguments, `validateSync(string, options)` expected");
        return;
    }

    if (!info[0]->IsString()) {
        Nan::ThrowTypeError("wrong 1st argument - string expected, `validateSync(string, options)`");
        return;
    }

    if (info.Length() == 2 && !info[1]->IsObject()) {
        Nan::ThrowTypeError("wrong 2nd argument - object expected, `validateSync(string, options)`");
        return;
    }

    // Get source data
    Nan::Utf8String sourceData(info[0]->ToString(context).ToLocalChecked());

    // Prepare options
    drafter_parse_options parseOptions = {false};

    if (info.Length() == 2) {
        OptionsResult *optionsResult = ParseOptionsObject(Local<Object>::Cast(info[1]), true);

        if (optionsResult->error != NULL) {
            Nan::ThrowTypeError(optionsResult->error);
        }

        parseOptions = optionsResult->parseOptions;
        FreeOptionsResult(&optionsResult);
    }

    // Parse the source data
    drafter_result *result;
    int err_code = drafter_check_blueprint(*sourceData,
                                           &result,
                                           parseOptions);
    if (err_code < 0) {
        Nan::ThrowError("Parsing failed");
        return;
    }

    if (result) {
        Local<Value> v8result = refract2v8(result, {true, DRAFTER_SERIALIZE_JSON});
        drafter_free_result(result);
        info.GetReturnValue().Set(v8result);
    }
    else {
        info.GetReturnValue().Set(Nan::Null());
    }
}
