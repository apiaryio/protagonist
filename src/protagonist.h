#include <node.h>
#include <v8.h>

#include "Blueprint.h"

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

        // Wraps snowcrash::blueprint
        static v8::Local<v8::Object> WrapBlueprint(const snowcrash::Blueprint& blueprint);

    private:
        Blueprint();
        ~Blueprint();

        static v8::Handle<v8::Value> New(const v8::Arguments& args);
        static v8::Persistent<v8::Function> constructor;
    };

    //
    // Parse function
    //
    extern v8::Handle<v8::Value> Parse(const v8::Arguments& args);

}