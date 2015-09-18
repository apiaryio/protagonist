#include "protagonist.h"
#include "SerializeResult.h"
#include "v8_wrapper.h"

using namespace v8;
using namespace protagonist;

SourceAnnotation::SourceAnnotation()
{
}

SourceAnnotation::~SourceAnnotation()
{
}

Nan::Persistent<Function> SourceAnnotation::constructor;

void SourceAnnotation::Init(Handle<Object> exports)
{
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
    t->SetClassName(Nan::New<String>("SourceAnnotation").ToLocalChecked());
    t->InstanceTemplate()->SetInternalFieldCount(1);

    constructor.Reset(t->GetFunction());
    exports->Set(Nan::New<String>("SourceAnnotation").ToLocalChecked(), t->GetFunction());
}

NAN_METHOD(SourceAnnotation::New)
{
    Nan::HandleScope scope;

    SourceAnnotation* annotation = ::new SourceAnnotation();
    annotation->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

Local<Object> SourceAnnotation::WrapSourceAnnotation(const snowcrash::SourceAnnotation& annotation)
{
    sos::Object annotationObject = drafter::WrapAnnotation(annotation);

    return v8_wrap(annotationObject)->ToObject();
}
