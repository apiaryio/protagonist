#include <string>
#include "protagonist.h"
#include "drafter.h"
#include "options.h"
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
    Options options;

    if (info.Length() == 2) {
        ErrorMessage err = ParseOptionsObject(options, Local<Object>::Cast(info[1]), true);

        if(!err.empty()) {
            Nan::ThrowTypeError(err.c_str());
        }
    }

    // Parse the source data
    drafter_result *result;
    int err_code = drafter_check_blueprint(*sourceData,
                                           &result,
                                           options.parseOptions());
    if (err_code < 0) {
        Nan::ThrowError("Parsing failed");
        return;
    }

    if (result) {
        Local<Value> v8result = refract2v8(result, true);
        drafter_free_result(result);
        info.GetReturnValue().Set(v8result);
    }
    else {
        info.GetReturnValue().Set(Nan::Null());
    }
}
