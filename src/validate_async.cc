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

namespace
{

    class ValidateWorker : public AsyncWorker
    {
        drafter_parse_options parseOptions;
        Nan::Utf8String* sourceData;
        Nan::Persistent<v8::Promise::Resolver>* persistent;

        // Output
        drafter_result* result;
        int parse_err_code;

    public:
        ValidateWorker(Callback* callback,
            Nan::Persistent<v8::Promise::Resolver>* persistent,
            drafter_parse_options parseOptions,
            Nan::Utf8String* sourceData)
            : AsyncWorker(callback)
            , parseOptions(parseOptions)
            , sourceData(sourceData)
            , persistent(persistent)
            , result(nullptr)
            , parse_err_code(-1)
        {
        }

        void Execute()
        {
            parse_err_code = drafter_check_blueprint(*(*sourceData), &result, parseOptions);
        }

        void HandleOKCallback()
        {
            Nan::HandleScope scope;

            // Callback args

            if (persistent) {
                auto resolver = Nan::New(*persistent);
                Local<Value> v8refract = Nan::Null();
                if (result) {
                    v8refract = refract2v8(result, { true, DRAFTER_SERIALIZE_JSON });
                }

                if (!parse_err_code) {
                    resolver->Resolve(Nan::GetCurrentContext(), v8refract);
                } else {
                    resolver->Reject(Nan::GetCurrentContext(), v8refract);
                }
                v8::Isolate::GetCurrent()->RunMicrotasks();
                return;
            } else if (callback) {
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
                            argv[1] = refract2v8(result, { true, DRAFTER_SERIALIZE_JSON });
                        } else {
                            argv[1] = Nan::Null();
                        }
                }
                callback->Call(2, argv);
                return;
            }
            Nan::ThrowTypeError("not handled OKCallback");
        }

        virtual ~ValidateWorker()
        {
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

} // namespace

static bool isLastParamCallback(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
    return info[info.Length() - 1]->IsFunction();
}

NAN_METHOD(protagonist::Validate)
{
    Nan::HandleScope scope;

    Nan::Persistent<v8::Promise::Resolver>* persistent = nullptr;
    Callback* callback = nullptr;
    v8::Local<v8::Promise::Resolver> resolver;

    size_t optionIndex = 0;
    size_t callbackIndex = 0;

    if (info.Length() < 1 || info.Length() > 3) {
        Nan::ThrowTypeError("wrong number of arguments, `parse(string, options, callback)` expected");
        return;
    }

    if (!info[0]->IsString()) {
        Nan::ThrowTypeError("wrong 1st argument - string expected");
        return;
    }

    if (isLastParamCallback(info)) { // last arg is callback funtion
        if (info.Length() == 3) {
            optionIndex = 1;
        }
        callbackIndex = info.Length() - 1;
    } else { // is Promise
        if (info.Length() == 2) {
            optionIndex = 1;
        } else if (info.Length() > 2) { // promise shoud not have more than 2 params
            Nan::ThrowSyntaxError("wrong number of arguments, `parse(string, [options])` expected");
            return;
        }
    }

    if (optionIndex && !info[optionIndex]->IsObject()) {
        Nan::ThrowTypeError("wrong 2nd argument - `options` expected `parse(string[, options][, callback])`");
        return;
    }

    drafter_parse_options parseOptions = { false };

    if (optionIndex) {
        OptionsResult* optionsResult = ParseOptionsObject(Handle<Object>::Cast(info[optionIndex]), true);

        if (optionsResult->error != NULL) {
            Nan::ThrowTypeError(optionsResult->error);
        }

        parseOptions = optionsResult->parseOptions;
        FreeOptionsResult(&optionsResult);
    }

    if (callbackIndex) {
        callback = new Callback(info[callbackIndex].As<Function>());
    } else {
        resolver = v8::Promise::Resolver::New(info.GetIsolate());
        persistent = new Nan::Persistent<v8::Promise::Resolver>(resolver);
    }

    AsyncQueueWorker(new ValidateWorker(callback, persistent, parseOptions, new Nan::Utf8String(info[0])));

    if (!callbackIndex) {
        info.GetReturnValue().Set(resolver->GetPromise());
    }
}
