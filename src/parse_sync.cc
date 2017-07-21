#include <string>
#include "protagonist.h"
#include "drafter.h"
#include "refractToV8.h"

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
    drafter_parse_options parseOptions = {false};
    drafter_serialize_options serializeOptions = {false, DRAFTER_SERIALIZE_JSON};

    if (info.Length() == 2) {
        OptionsResult *optionsResult = ParseOptionsObject(Handle<Object>::Cast(info[1]), false);

        if (optionsResult->error != NULL) {
            Nan::ThrowTypeError(optionsResult->error);
        }

        parseOptions = optionsResult->parseOptions;
        serializeOptions = optionsResult->serializeOptions;
        FreeOptionsResult(&optionsResult);
    }

    // Parse the source data
    drafter_result *result;
    int err_code = drafter_parse_blueprint(*sourceData,
                                           &result,
                                           parseOptions);
    switch (err_code) {
    case DRAFTER_EUNKNOWN:
        Nan::ThrowError("Parser: Unknown Error");
        break;
    case DRAFTER_EINVALID_INPUT:
        Nan::ThrowError("Parser: Invalid Input");
        break;
    case DRAFTER_EINVALID_OUTPUT:
        Nan::ThrowError("Parser: Invalid Output");
        break;
    default:
        break;
    }

    Local<Value> v8result = refract2v8(result, serializeOptions);
    info.GetReturnValue().Set(v8result);
}
