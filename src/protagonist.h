#ifndef PROTAGONIST_H
#define PROTAGONIST_H

#include <node.h>
#include <v8.h>
#include "nan.h"
#include "Serialize.h"
#include "SerializeResult.h"
#include "SourceAnnotation.h"

namespace protagonist {

    //
    // Options parsing
    ///
    struct OptionsResult {
      snowcrash::BlueprintParserOptions options;
      drafter::ASTType astType;
      const char *error;
    };

    OptionsResult* ParseOptionsObject(v8::Handle<v8::Object>);

    //
    // SourceAnnotation
    //
    class SourceAnnotation : public Nan::ObjectWrap {
    public:
        static void Init(v8::Handle<v8::Object> target);

        // Wraps snowcrash::SourceAnnotation
        static v8::Local<v8::Object> WrapSourceAnnotation(const snowcrash::SourceAnnotation& annotation);

    private:
        SourceAnnotation();
        ~SourceAnnotation();

        static NAN_METHOD(New);
        static Nan::Persistent<v8::Function> constructor;
    };

    //
    // Result
    //
    class Result : public Nan::ObjectWrap {
    public:
        static void Init(v8::Handle<v8::Object> target);

        // Wraps snowcrash::Warnings and snowcrash:Blueprint into report object
        // Note: snowcrash::Result::Error is being sent separately as Error object
        static v8::Local<v8::Object> WrapResult(snowcrash::ParseResult<snowcrash::Blueprint>& parseResult,
                                                const snowcrash::BlueprintParserOptions& options,
                                                const drafter::ASTType& astType);

    private:
        Result();
        ~Result();

        static NAN_METHOD(New);
        static Nan::Persistent<v8::Function> constructor;
    };

    //
    // Parse function
    //
    extern NAN_METHOD(Parse);
    extern NAN_METHOD(ParseSync);
}

#endif
