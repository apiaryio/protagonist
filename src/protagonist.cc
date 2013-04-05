#include <node.h>
#include <v8.h>
#include "protagonist.h"

using namespace v8;
using namespace protagonist;

void Init(Handle<Object> exports, Handle<Object> module) {

    // Blueprint object
    Blueprint::Init(exports);

    // Parse function
    exports->Set(String::NewSymbol("parse"), FunctionTemplate::New(Parse)->GetFunction());
}

NODE_MODULE(protagonist, Init)
