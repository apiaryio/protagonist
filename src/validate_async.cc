#include <string>
#include "protagonist.h"
#include "refractToV8.h"

using std::string;
using namespace v8;
using namespace protagonist;

using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;

namespace {

class ValidateWorker : public AsyncWorker {
    drafter_parse_options parseOptions;
    Nan::Utf8String *sourceData;

    // Output
    drafter_result *result;
    int parse_err_code;

   public:
    ValidateWorker(Callback *callback, drafter_parse_options parseOptions,
                   Nan::Utf8String *sourceData)
        : AsyncWorker(callback),
          parseOptions(parseOptions),
          sourceData(sourceData),
          result(nullptr),
          parse_err_code(-1) {}

    void Execute() {
        parse_err_code =
            drafter_check_blueprint(*(*sourceData), &result, parseOptions);
    }

    void HandleOKCallback() {
        Nan::HandleScope scope;

        // Error Object
        Local<Value> argv[2];

        switch (parse_err_code) {
            case DRAFTER_EUNKNOWN:
                argv[0] = Nan::Error("Parser: Unknown Error");
                argv[1] = Nan::Null();
                break;
            case DRAFTER_EINVALID_INPUT:
                argv[0] = Nan::Error("Parser: Invalid Input");
                argv[1] = Nan::Null();
                break;
            case DRAFTER_EINVALID_OUTPUT:
                argv[0] = Nan::Error("Parser: Invalid Output");
                argv[1] = Nan::Null();
                break;
            default:
                argv[0] = Nan::Null();
                if (result) {
                    argv[1] =
                        refract2v8(result, {true, DRAFTER_SERIALIZE_JSON});
                } else {
                    argv[1] = Nan::Null();
                }
        }

        callback->Call(2, argv);
    }

    virtual ~ValidateWorker() {
        if (result) {
            drafter_free_result(result);
            result = nullptr;
        }

        if (sourceData) {
            delete sourceData;
            sourceData = nullptr;
        }
    }
};
}

NAN_METHOD(protagonist::Validate) {
    Nan::HandleScope scope;

    // Check arguments
    if (info.Length() != 2 && info.Length() != 3) {
        Nan::ThrowTypeError(
            "wrong number of arguments, `validate(string, options, callback)` "
            "expected");
        return;
    }

    if (!info[0]->IsString()) {
        Nan::ThrowTypeError(
            "wrong argument - string expected, `validate(string, options, "
            "callback)`");
        return;
    }

    if ((info.Length() == 2 && !info[1]->IsFunction()) ||
        (info.Length() == 3 && !info[2]->IsFunction())) {
        Nan::ThrowTypeError(
            "wrong argument - callback expected, `validate(string, options, "
            "callback)`");
        return;
    }

    if (info.Length() == 3 && !info[1]->IsObject()) {
        Nan::ThrowTypeError(
            "wrong argument - object expected, `validate(string, options, "
            "callback)`");
        return;
    }

    // Prepare options
    drafter_parse_options parseOptions = {false};

    if (info.Length() == 3) {
        OptionsResult *optionsResult =
            ParseOptionsObject(Handle<Object>::Cast(info[1]), true);

        if (optionsResult->error != NULL) {
            Nan::ThrowTypeError(optionsResult->error);
        }

        parseOptions = optionsResult->parseOptions;
        FreeOptionsResult(&optionsResult);
    }

    // Get Callback
    Callback *callback = new Callback(
        info.Length() == 3 ? info[2].As<Function>() : info[1].As<Function>());

    AsyncQueueWorker(new ValidateWorker(callback, parseOptions,
                                        new Nan::Utf8String(info[0])));
}
