#include "protagonist.h"

using namespace v8;
using namespace protagonist;

Blueprint::Blueprint()
{
}

Blueprint::~Blueprint()
{
}

Persistent<Function> Blueprint::constructor;

void Blueprint::Init(Handle<Object> exports)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(String::NewSymbol("Blueprint"));

    constructor = Persistent<Function>::New(t->GetFunction());
    exports->Set(String::NewSymbol("Blueprint"), constructor);
}

Handle<Value> Blueprint::New(const Arguments& args)
{
    HandleScope scope;

    Blueprint* blueprint = ::new Blueprint();
    blueprint->Wrap(args.This());

    return scope.Close(args.This());
}

Handle<Value> Blueprint::NewInstance()
{
    HandleScope scope;

    Local<Object> instance = constructor->NewInstance();

    return scope.Close(instance);
}

Local<Object> Blueprint::WrapBlueprint(const snowcrash::Blueprint& blueprint)
{
    Local<Object> blueprintWrap = constructor->NewInstance();

    // Wrap snowcrash Blueprint AST
    blueprintWrap->Set(String::NewSymbol("name"), String::New(blueprint.name.c_str()));    

    return blueprintWrap;
}

// Handle<Value> Blueprint::apiName(const Arguments& args)
// {
//     HandleScope scope;
//     Blueprint* obj = ObjectWrap::Unwrap<Blueprint>(args.This());
//     //obj->m_blueprint->name.c_str()
//     return scope.Close(String::New("xx"));
// }
