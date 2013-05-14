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
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(String::NewSymbol("SourceAnnotation"));

    constructor = Persistent<Function>::New(t->GetFunction());
    exports->Set(String::NewSymbol("SourceAnnotation"), constructor);
}

Handle<Value> SourceAnnotation::New(const Arguments& args)
{
    HandleScope scope;
    SourceAnnotation* annotation = ::new SourceAnnotation();
    annotation->Wrap(args.This());
    return scope.Close(args.This());
}

Handle<Value> SourceAnnotation::NewInstance()
{
    HandleScope scope;
    Local<Object> instance = constructor->NewInstance();
    return scope.Close(instance);
}

static Local<Object> WrapSourceDataRange(const snowcrash::SourceDataRange& range) 
{
    Local<Object> rangeObject = Object::New();

    rangeObject->Set(String::NewSymbol("index"), Number::New(range.location));
    rangeObject->Set(String::NewSymbol("length"), Number::New(range.length));
    return rangeObject;
}

Local<Object> SourceAnnotation::WrapSourceAnnotation(const snowcrash::SourceAnnotation& annotation)
{
    Local<Object> annotationWrap = constructor->NewInstance();

    annotationWrap->Set(String::NewSymbol("code"), Number::New(annotation.code));
    annotationWrap->Set(String::NewSymbol("message"), String::New(annotation.message.c_str()));

    Local<Object> location = Array::New(annotation.location.size());
    size_t i = 0;
    for (snowcrash::SourceDataBlock::const_iterator it = annotation.location.begin(); 
         it != annotation.location.end();
         ++it, ++i) {

        location->Set(i, WrapSourceDataRange(*it));
    }

    annotationWrap->Set(String::NewSymbol("location"), location);    
    return annotationWrap;
}
