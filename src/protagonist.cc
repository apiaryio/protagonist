#include "protagonist.h"

using namespace v8;
using namespace protagonist;

void Init(Handle<Object> exports) {

    // parse function
    exports->Set(Nan::New<String>("parse").ToLocalChecked(), Nan::New<FunctionTemplate>(Parse)->GetFunction());

    // parseSync function
    exports->Set(Nan::New<String>("parseSync").ToLocalChecked(), Nan::New<FunctionTemplate>(ParseSync)->GetFunction());

    // validate function
    exports->Set(Nan::New<String>("validate").ToLocalChecked(), Nan::New<FunctionTemplate>(Validate)->GetFunction());
    // validateSync function
    exports->Set(Nan::New<String>("validateSync").ToLocalChecked(), Nan::New<FunctionTemplate>(ValidateSync)->GetFunction());
}

NODE_MODULE(protagonist, Init)
