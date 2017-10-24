#include <string>
#include "drafter.h"
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

class ParseWorker : public AsyncWorker {
    drafter_parse_options parseOptions;
    drafter_serialize_options serializeOptions;
    Nan::Utf8String *sourceData;

    // Output
    drafter_result *result;
    int parse_err_code;

   public:
    ParseWorker(Callback *callback, drafter_parse_options parseOptions,
                drafter_serialize_options serializeOptions,
                Nan::Utf8String *sourceData)
        : AsyncWorker(callback),
          parseOptions(parseOptions),
          serializeOptions(serializeOptions),
          sourceData(sourceData),
          result(nullptr),
          parse_err_code(-1) {}

    void Execute() {
        parse_err_code =
            drafter_parse_blueprint(*(*sourceData), &result, parseOptions);
    }

    void HandleOKCallback() {
        Nan::HandleScope scope;

        v8::Local<v8::Value> argv[] = {Nan::Null(),
                                       refract2v8(result, serializeOptions)};

        if (0 != parse_err_code) {
            argv[0] = annotations2v8(result);
        }

        callback->Call(2, argv);
    }

    virtual ~ParseWorker() {
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

NAN_METHOD(protagonist::Parse) {
    Nan::HandleScope scope;

    // Check arguments
    if (info.Length() != 2 && info.Length() != 3) {
        Nan::ThrowTypeError(
            "wrong number of arguments, `parse(string, options, callback)` "
            "expected");
        return;
    }

    if (!info[0]->IsString()) {
        Nan::ThrowTypeError(
            "wrong argument - string expected, `parse(string, options, "
            "callback)`");
        return;
    }

    if ((info.Length() == 2 && !info[1]->IsFunction()) ||
        (info.Length() == 3 && !info[2]->IsFunction())) {
        Nan::ThrowTypeError(
            "wrong argument - callback expected, `parse(string, options, "
            "callback)`");
        return;
    }

    if (info.Length() == 3 && !info[1]->IsObject()) {
        Nan::ThrowTypeError(
            "wrong argument - object expected, `parse(string, options, "
            "callback)`");
        return;
    }

    // Prepare options
    drafter_parse_options parseOptions = {false};
    drafter_serialize_options serializeOptions = {false,
                                                  DRAFTER_SERIALIZE_JSON};

    if (info.Length() == 3) {
        OptionsResult *optionsResult =
            ParseOptionsObject(Handle<Object>::Cast(info[1]), false);

        if (optionsResult->error != NULL) {
            Nan::ThrowTypeError(optionsResult->error);
        }

        parseOptions = optionsResult->parseOptions;
        serializeOptions = optionsResult->serializeOptions;
        FreeOptionsResult(&optionsResult);
    }

    Callback *callback = new Callback(
        info.Length() == 3 ? info[2].As<Function>() : info[1].As<Function>());

    AsyncQueueWorker(new ParseWorker(callback, parseOptions, serializeOptions,
                                     new Nan::Utf8String(info[0])));
}
