#ifndef PROTAGONIST_H
#define PROTAGONIST_H

#include <node.h>
#include <v8.h>
#include "nan.h"
#include "Blueprint.h"
#include "BlueprintSourcemap.h"
#include "SectionParserData.h"
#include "SourceAnnotation.h"

namespace protagonist {

    //
    // SourceAnnotation
    //
    class SourceAnnotation : public node::ObjectWrap {
    public:
        static void Init(v8::Handle<v8::Object> target);

        // Wraps snowcrash::SourceAnnotation
        static v8::Local<v8::Object> WrapSourceAnnotation(const snowcrash::SourceAnnotation& annotation);

    private:
        SourceAnnotation();
        ~SourceAnnotation();

        static NAN_METHOD(New);
        static v8::Persistent<v8::Function> constructor;
    };

    //
    // Result
    //
    class Result : public node::ObjectWrap {
    public:
        static void Init(v8::Handle<v8::Object> target);

        // Wraps snowcrash::Warnings and snowcrash:Blueprint into report object
        // Note: snowcrash::Result::Error is being sent separately as Error object
        static v8::Local<v8::Object> WrapResult(const snowcrash::Report& report,
                                                const snowcrash::Blueprint& blueprint,
                                                const snowcrash::SourceMap<snowcrash::Blueprint>& sourcemap,
                                                const snowcrash::BlueprintParserOptions& options);

    private:
        Result();
        ~Result();

        static NAN_METHOD(New);
        static v8::Persistent<v8::Function> constructor;
    };

    //
    // Parse function
    //
    extern NAN_METHOD(Parse);
}

#endif
