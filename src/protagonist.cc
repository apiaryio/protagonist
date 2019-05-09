#include "protagonist.h"

using namespace v8;
using namespace protagonist;

void Init(Local<Object> exports) {
    Local<Context> context = Nan::GetCurrentContext();

    // parse function
    exports->Set(context, Nan::New<String>("parse").ToLocalChecked(), Nan::New<FunctionTemplate>(Parse)->GetFunction(context).ToLocalChecked());

    // parseSync function
    exports->Set(context, Nan::New<String>("parseSync").ToLocalChecked(), Nan::New<FunctionTemplate>(ParseSync)->GetFunction(context).ToLocalChecked());

    // validate function
    exports->Set(context, Nan::New<String>("validate").ToLocalChecked(), Nan::New<FunctionTemplate>(Validate)->GetFunction(context).ToLocalChecked());
    // validateSync function
    exports->Set(context, Nan::New<String>("validateSync").ToLocalChecked(), Nan::New<FunctionTemplate>(ValidateSync)->GetFunction(context).ToLocalChecked());
}

NODE_MODULE(protagonist, Init)
