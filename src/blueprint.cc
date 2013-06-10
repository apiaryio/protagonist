#include "protagonist.h"
#include "Serialize.h"

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

static Local<Object> WrapKeyValuePair(const snowcrash::KeyValuePair& kvpair)
{
    Local<Object> keyValueObject = Object::New();

    keyValueObject->Set(String::NewSymbol("name"), String::New(kvpair.first.c_str()));
    keyValueObject->Set(String::NewSymbol("value"), String::New(kvpair.second.c_str()));

    return keyValueObject;
}

static Local<Object> WrapMetadata(const snowcrash::Collection<snowcrash::Metadata>::type& metadataCollection) 
{
    Local<Object> metadataCollectionObject = Array::New(metadataCollection.size());
    size_t i = 0;
    for (snowcrash::Collection<snowcrash::Metadata>::const_iterator it = metadataCollection.begin(); 
         it != metadataCollection.end();
         ++it, ++i) {

        metadataCollectionObject->Set(i, WrapKeyValuePair(*it));
    }

    return metadataCollectionObject;
}

static Local<Object> WrapHeaders(const snowcrash::Collection<snowcrash::Header>::type& headers) 
{
    Local<Object> headersObject = Array::New(headers.size());
    size_t i = 0;
    for (snowcrash::Collection<snowcrash::Header>::const_iterator it = headers.begin(); 
         it != headers.end();
         ++it, ++i) {

        headersObject->Set(i, WrapKeyValuePair(*it));
    }

    return headersObject;
}

static Local<Object> WrapPayload(const snowcrash::Payload& payload) 
{
    Local<Object> payloadObject = Object::New();

    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), String::New(payload.name.c_str()));
    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), String::New(payload.description.c_str()));

    // TODO: Parameters

    // Headers
    Local<Object> headers = WrapHeaders(payload.headers);
    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Headers.c_str()), headers);

    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Body.c_str()), String::New(payload.body.c_str()));
    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Schema.c_str()), String::New(payload.schema.c_str()));    

    return payloadObject;
}

static Local<Object> WrapMethod(const snowcrash::Method& method) 
{
    Local<Object> methodObject = Object::New();

    methodObject->Set(String::NewSymbol(snowcrash::SerializeKey::Method.c_str()), String::New(method.method.c_str()));
    methodObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), String::New(method.name.c_str()));
    methodObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), String::New(method.description.c_str()));

    // TODO: Parameters

    // Headers
    Local<Object> headers = WrapHeaders(method.headers);
    methodObject->Set(String::NewSymbol(snowcrash::SerializeKey::Headers.c_str()), headers);

    // Requests
    Local<Object> requests = Array::New(method.requests.size());
    size_t i = 0;
    for (snowcrash::Collection<snowcrash::Request>::const_iterator it = method.requests.begin(); 
         it != method.requests.end();
         ++it, ++i) {

        requests->Set(i, WrapPayload(*it));
    }
    methodObject->Set(String::NewSymbol(snowcrash::SerializeKey::Requests.c_str()), requests);

    // Responses
    Local<Object> responses = Array::New(method.responses.size());
    i = 0;
    for (snowcrash::Collection<snowcrash::Response>::const_iterator it = method.responses.begin(); 
         it != method.responses.end();
         ++it, ++i) {

        responses->Set(i, WrapPayload(*it));
    }
    methodObject->Set(String::NewSymbol(snowcrash::SerializeKey::Responses.c_str()), responses);

    return methodObject;
}

static Local<Object> WrapResource(const snowcrash::Resource& resource) 
{
    Local<Object> resourceObject = Object::New();

    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::URITemplate.c_str()), String::New(resource.uriTemplate.c_str()));
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), String::New(resource.name.c_str()));
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), String::New(resource.description.c_str()));

    // TODO: Parameters

    // Headers
    Local<Object> headers = WrapHeaders(resource.headers);
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Headers.c_str()), headers);

    // Object
    Local<Object> object = WrapPayload(resource.object);
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Object.c_str()), object);

    // Methods
    Local<Object> methods = Array::New(resource.methods.size());
    size_t i = 0;
    for (snowcrash::Collection<snowcrash::Method>::const_iterator it = resource.methods.begin(); 
         it != resource.methods.end();
         ++it, ++i) {

        methods->Set(i, WrapMethod(*it));
    }
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Methods.c_str()), methods);

    return resourceObject;
}

static Local<Object> WrapResourceGroup(const snowcrash::ResourceGroup& group) 
{
    Local<Object> groupObject = Object::New();

    groupObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), String::New(group.name.c_str()));
    groupObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), String::New(group.description.c_str()));

    Local<Object> resources = Array::New(group.resources.size());
    size_t i = 0;
    for (snowcrash::Collection<snowcrash::Resource>::const_iterator it = group.resources.begin(); 
         it != group.resources.end();
         ++it, ++i) {

        resources->Set(i, WrapResource(*it));
    }
    groupObject->Set(String::NewSymbol(snowcrash::SerializeKey::Resources.c_str()), resources);    

    return groupObject;
}

Local<Object> Blueprint::WrapBlueprint(const snowcrash::Blueprint& blueprint)
{
    Local<Object> blueprintWrap = constructor->NewInstance();

    // Wrap snowcrash Blueprint AST

    // Metadata
    Local<Object> metadata = WrapMetadata(blueprint.metadata);
    blueprintWrap->Set(String::NewSymbol(snowcrash::SerializeKey::Metadata.c_str()), metadata);    

    // Blueprint Name
    blueprintWrap->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), String::New(blueprint.name.c_str()));

    // Blueprint Description
    blueprintWrap->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), String::New(blueprint.description.c_str()));

    // Resource Groups
    Local<Object> groups = Array::New(blueprint.resourceGroups.size());
    size_t i = 0;
    for (snowcrash::Collection<snowcrash::ResourceGroup>::const_iterator it = blueprint.resourceGroups.begin(); 
         it != blueprint.resourceGroups.end();
         ++it, ++i) {

        groups->Set(i, WrapResourceGroup(*it));
    }
    blueprintWrap->Set(String::NewSymbol(snowcrash::SerializeKey::ResourceGroups.c_str()), groups);

    return blueprintWrap;
}
