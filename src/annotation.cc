#include "protagonist.h"
#include "Serialize.h"

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

    constructor = Persistent<Function>::New(t->GetFunction());
    exports->Set(NanNew<String>("SourceAnnotation"), constructor);
}

NAN_METHOD(SourceAnnotation::New)
{
    NanScope();

    SourceAnnotation* annotation = ::new SourceAnnotation();
    annotation->Wrap(args.This());

    NanReturnValue(args.This());
}

static Local<Object> WrapSourceCharactersRange(const mdp::CharactersRange& range)
{
    Local<Object> rangeObject = NanNew<Object>();

    rangeObject->Set(NanNew<String>("index"), NanNew<Number>(range.location));
    rangeObject->Set(NanNew<String>("length"), NanNew<Number>(range.length));
    return rangeObject;
}

Local<Object> SourceAnnotation::WrapSourceAnnotation(const snowcrash::SourceAnnotation& annotation)
{
    Local<Object> annotationWrap = constructor->NewInstance();

    annotationWrap->Set(NanNew<String>("code"), NanNew<Number>(annotation.code));
    annotationWrap->Set(NanNew<String>("message"), NanNew<String>(annotation.message.c_str()));

    Local<Object> location = NanNew<Array>(annotation.location.size());
    size_t i = 0;
    for (mdp::CharactersRangeSet::const_iterator it = annotation.location.begin();
         it != annotation.location.end();
         ++it, ++i) {

        location->Set(i, WrapSourceCharactersRange(*it));
    }

    annotationWrap->Set(NanNew<String>("location"), location);
    return annotationWrap;
}
