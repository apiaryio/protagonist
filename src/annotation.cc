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

Persistent<Function> SourceAnnotation::constructor;

void SourceAnnotation::Init(Handle<Object> exports)
{
    NanScope();

    Local<FunctionTemplate> t = NanNew<FunctionTemplate>(New);
    t->SetClassName(NanNew<String>("SourceAnnotation"));
    t->InstanceTemplate()->SetInternalFieldCount(1);

    NanAssignPersistent<Function>(constructor, t->GetFunction());
    exports->Set(NanNew<String>("SourceAnnotation"), t->GetFunction());
}

NAN_METHOD(SourceAnnotation::New)
{
    NanScope();

    SourceAnnotation* annotation = ::new SourceAnnotation();
    annotation->Wrap(args.This());

    NanReturnValue(args.This());
}

Local<Object> SourceAnnotation::WrapSourceAnnotation(const snowcrash::SourceAnnotation& annotation)
{
    sos::Object annotationObject = drafter::WrapAnnotation(annotation);

    return v8_wrap(annotationObject)->ToObject();
}
