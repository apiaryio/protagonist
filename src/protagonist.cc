#include "protagonist.h"

using namespace v8;
using namespace protagonist;

void Init(Handle<Object> exports) {

    // SourceAnnotation object
    SourceAnnotation::Init(exports);

    // Result object
    Result::Init(exports);

    // Parse function
    exports->Set(Nan::New<String>("parse").ToLocalChecked(), Nan::New<FunctionTemplate>(Parse)->GetFunction());
    exports->Set(Nan::New<String>("parseSync").ToLocalChecked(), Nan::New<FunctionTemplate>(ParseSync)->GetFunction());

    // Validate function
    exports->Set(Nan::New<String>("validate").ToLocalChecked(), Nan::New<FunctionTemplate>(Validate)->GetFunction());
    exports->Set(Nan::New<String>("validateSync").ToLocalChecked(), Nan::New<FunctionTemplate>(ValidateSync)->GetFunction());
}

NODE_MODULE(protagonist, Init)
