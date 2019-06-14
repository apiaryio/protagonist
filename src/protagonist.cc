#include "protagonist.h"

using namespace v8;
using namespace protagonist;

NAN_MODULE_INIT(InitAll) {
    Nan::Set(target, Nan::New<String>("parse").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(Parse)).ToLocalChecked());

    Nan::Set(target, Nan::New<String>("parseSync").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(ParseSync)).ToLocalChecked());

    Nan::Set(target, Nan::New<String>("validate").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(Validate)).ToLocalChecked());

    Nan::Set(target, Nan::New<String>("validateSync").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(ValidateSync)).ToLocalChecked());
}

NODE_MODULE(protagonist, InitAll)
