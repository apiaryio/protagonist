#include <node.h>
#include <v8.h>
#include "Blueprint.h"
#include "BlueprintSourcemap.h"
#include "SectionParserData.h"
#include "SourceAnnotation.h"

namespace protagonist {

    //
    // Blueprint wrapper
    //
    class Blueprint : public node::ObjectWrap {
    public:
        // Blueprint Wrapper registration
        static void Init(v8::Handle<v8::Object> target);

        // Convenience function for constructor->NewInstance
        static v8::Handle<v8::Value> NewInstance(); 

        // Wraps snowcrash:Blueprint
        static v8::Local<v8::Object> WrapBlueprint(const snowcrash::Blueprint& blueprint);

    private:
        Blueprint();
        ~Blueprint();

        static v8::Handle<v8::Value> New(const v8::Arguments& args);
        static v8::Persistent<v8::Function> constructor;
    };

    //
    // Sourcemap wrapper
    //
    class Sourcemap : public node::ObjectWrap {
    public:
        // Sourcemap Wrapper registration
        static void Init(v8::Handle<v8::Object> target);

        // Convenience function for constructor->NewInstance
        static v8::Handle<v8::Value> NewInstance(); 

        // Wraps source map of snowcrash:Blueprint
        static v8::Local<v8::Object> WrapBlueprint(const snowcrash::SourceMap<snowcrash::Blueprint>& blueprint);

    private:
        Sourcemap();
        ~Sourcemap();

        static v8::Handle<v8::Value> New(const v8::Arguments& args);
        static v8::Persistent<v8::Function> constructor;
    };

    //
    // SourceAnnotation 
    //
    class SourceAnnotation : public node::ObjectWrap {
    public:
        static void Init(v8::Handle<v8::Object> target);
        static v8::Handle<v8::Value> NewInstance(); 

        // Wraps snowcrash::SourceAnnotation
        static v8::Local<v8::Object> WrapSourceAnnotation(const snowcrash::SourceAnnotation& annotation);

    private:
        SourceAnnotation();
        ~SourceAnnotation();

        static v8::Handle<v8::Value> New(const v8::Arguments& args);
        static v8::Persistent<v8::Function> constructor;
    };

    //
    // Result 
    //
    class Result : public node::ObjectWrap {
    public:
        static void Init(v8::Handle<v8::Object> target);
        static v8::Handle<v8::Value> NewInstance(); 

        // Wraps snowcrash::Warnings and snowcrash:Blueprint into report object
        // Note: snowcrash::Result::Error is being sent separately as Error object
        static v8::Local<v8::Object> WrapResult(const snowcrash::Report& report,
                                                const snowcrash::Blueprint& blueprint,
                                                const snowcrash::SourceMap<snowcrash::Blueprint>& sourcemap,
                                                const snowcrash::BlueprintParserOptions& options);

    private:
        Result();
        ~Result();

        static v8::Handle<v8::Value> New(const v8::Arguments& args);
        static v8::Persistent<v8::Function> constructor;
    };    

    //
    // Parse function
    //
    extern v8::Handle<v8::Value> Parse(const v8::Arguments& args);

}
