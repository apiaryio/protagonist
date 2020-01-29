#include <string>
#include "drafter.h"
#include "protagonist.h"
#include "refractToV8.h"
#include "options.h"

#include <assert.h>

using std::string;
using namespace v8;
using namespace protagonist;

using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;

namespace
{
    class ParseWorker : public AsyncWorker
    {
        Options options;
        Nan::Utf8String* sourceData;

        Nan::Persistent<v8::Promise::Resolver>* persistent;

        // Output
        drafter_result* result;
        int parse_err_code;

     public:
        ParseWorker(Callback* callback,
            Nan::Persistent<v8::Promise::Resolver>* persistent,
            Options opts,
            Nan::Utf8String* sourceData)
            : AsyncWorker(callback)
            , options(std::move(opts))
            , sourceData(sourceData)
            , persistent(persistent)
            , result(nullptr)
            , parse_err_code(-1)
        {
        }

        void Execute()
        {
            parse_err_code = drafter_parse_blueprint(*(*sourceData), &result, options.parseOptions());
        }

        void HandleOKCallback()
        {
            Nan::HandleScope scope;
            Local<Value> error = Nan::Null();
            Local<Value> v8refract = Nan::Null();

            switch (parse_err_code) {
                case DRAFTER_EUNKNOWN:
                    error = Nan::Error("Parser: Unknown Error");
                    break;
                case DRAFTER_EINVALID_INPUT:
                    error = Nan::Error("Parser: Invalid Input");
                    break;
                case DRAFTER_EINVALID_OUTPUT:
                    error = Nan::Error("Parser: Invalid Output");
                    break;
                default:
                    // do nothing
                    ;
            }

            if (result) {
                v8refract = refract2v8(result, options.serializeSourcemaps());
            }

            if (persistent) {
                auto resolver = Nan::New(*persistent);

                if (parse_err_code >= 0) {
                    resolver->Resolve(Nan::GetCurrentContext(), v8refract);
                } else {
                    resolver->Reject(Nan::GetCurrentContext(), error);
                }

                v8::Isolate::GetCurrent()->RunMicrotasks();
                return;
            } else if (callback) {
                Local<Value> argv[] = { Nan::Null(), Nan::Null() };

                if (parse_err_code >= 0) {
                    argv[1] = v8refract;
                } else {
                    argv[0] = error;
                }

                callback->Call(2, argv);
                return;
            }

            // this should never happen unless sent nullptr to `callback` and `persitent` from calling function
            assert(0);
        }

        virtual ~ParseWorker()
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
}

/**
 * posible args variant
 * (source) -> promise
 * (source, options) -> promise
 * (source, callback) -> callback
 * (source, option, callback) -> callback
 */

static bool isLastParamCallback(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
    return info[info.Length() - 1]->IsFunction();
}

NAN_METHOD(protagonist::Parse)
{
    Nan::HandleScope scope;
    Nan::Persistent<v8::Promise::Resolver>* persistent = nullptr;
    Callback* callback = nullptr;
    v8::Local<v8::Promise::Resolver> resolver;

    // Check arguments

    size_t optionIndex = 0;
    size_t callbackIndex = 0;

    if (info.Length() < 1 || info.Length() > 3) {
        Nan::ThrowTypeError("wrong number of arguments, `parse(string, options, callback)` expected");
        return;
    }

    if (!info[0]->IsString()) {
        Nan::ThrowTypeError("wrong 1st argument - string expected, `parse(string, options, [callback])`");
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
            Nan::ThrowTypeError("wrong number of arguments, `parse(string, options)` expected");
            return;
        }
    }

    if (optionIndex && !info[optionIndex]->IsObject()) {
        Nan::ThrowTypeError("wrong 2nd argument - object expected, `parse(string, options, [callback])`");
        return;
    }

    // Prepare options
    Options options{};

    if (optionIndex) {
        ErrorMessage msg = ParseOptionsObject(options, Local<Object>::Cast(info[optionIndex]), false);

        if (!msg.empty()) {
            Nan::ThrowTypeError(msg.c_str());
            return;
        }
    }

    if (callbackIndex) {
        callback = new Callback(info[callbackIndex].As<Function>());
    } else {
        resolver = v8::Promise::Resolver::New(Nan::GetCurrentContext()).ToLocalChecked();
        persistent = new Nan::Persistent<v8::Promise::Resolver>(resolver);
    }

    AsyncQueueWorker(
        new ParseWorker(callback, persistent, std::move(options), new Nan::Utf8String(info[0])));

    if (!callbackIndex) {
        info.GetReturnValue().Set(resolver->GetPromise());
    }
}
