#include "protagonist.h"
#include "Serialize.h"

using namespace v8;
using namespace protagonist;

Sourcemap::Sourcemap()
{
}

Sourcemap::~Sourcemap()
{
}

Persistent<Function> Sourcemap::constructor;

void Sourcemap::Init(Handle<Object> exports)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(String::NewSymbol("Sourcemap"));

    constructor = Persistent<Function>::New(t->GetFunction());
    exports->Set(String::NewSymbol("Sourcemap"), constructor);
}

Handle<Value> Sourcemap::New(const Arguments& args)
{
    HandleScope scope;

    Sourcemap* sourcemap = ::new Sourcemap();
    sourcemap->Wrap(args.This());

    return scope.Close(args.This());
}

Handle<Value> Sourcemap::NewInstance()
{
    HandleScope scope;

    Local<Object> instance = constructor->NewInstance();

    return scope.Close(instance);
}

/** Wrap source map */
static Local<Object> WrapSourceMap(const snowcrash::SourceMapBase& value)
{
    Local<Object> sourceMap = Array::New(value.sourceMap.size());
    size_t i = 0;

    for (mdp::RangeSet<mdp::BytesRange>::const_iterator it = value.sourceMap.begin();
         it != value.sourceMap.end();
         ++it, ++i) {

        Local<Object> range = Array::New(2);
        
        range->Set(0, Number::New(it->location));
        range->Set(1, Number::New(it->length));

        sourceMap->Set(i, range);
    }

    return sourceMap;
}

/** Wrap source map collection */
static Local<Object> WrapSourceMaps(const snowcrash::Collection<snowcrash::SourceMap<snowcrash::KeyValuePair> >::type& collection)
{
    Local<Object> sourceMapsArray = Array::New(collection.size());
    size_t i = 0;

    for (snowcrash::Collection<snowcrash::SourceMap<snowcrash::KeyValuePair> >::const_iterator it = collection.begin();
         it != collection.end();
         ++it, ++i) {

        sourceMapsArray->Set(i, WrapSourceMap(*it));
    }

    return sourceMapsArray;
}

/** Wrap Parameters source map */
static Local<Object> WrapParameters(const snowcrash::SourceMap<snowcrash::Parameters>& parameters)
{
    Local<Object> parametersCollectionArray = Array::New(parameters.list.size());
    size_t i = 0;

    for (snowcrash::Collection<snowcrash::SourceMap<snowcrash::Parameter> >::const_iterator it = parameters.list.begin();
         it != parameters.list.end();
         ++it, ++i) {

        Local<Object> parameterObject = Object::New();

        // Name
        parameterObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), WrapSourceMap(it->name));

        // Description
        parameterObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), WrapSourceMap(it->description));

        // Type
        parameterObject->Set(String::NewSymbol(snowcrash::SerializeKey::Type.c_str()), WrapSourceMap(it->type));

        // Use flag
        parameterObject->Set(String::NewSymbol(snowcrash::SerializeKey::Required.c_str()), WrapSourceMap(it->use));

        // Default value
        parameterObject->Set(String::NewSymbol(snowcrash::SerializeKey::Default.c_str()), WrapSourceMap(it->defaultValue));

        // Example value
        parameterObject->Set(String::NewSymbol(snowcrash::SerializeKey::Example.c_str()), WrapSourceMap(it->exampleValue));

        // Values
        Local<Object> valuesCollectionArray = Array::New(it->values.list.size());
        size_t j = 0;

        for (snowcrash::Collection<snowcrash::SourceMap<snowcrash::Value> >::const_iterator val_it = it->values.list.begin();
             val_it != it->values.list.end();
            ++val_it, ++j) {

            Local<Object> valueObject = Object::New();

            // Value
            valueObject->Set(String::NewSymbol(snowcrash::SerializeKey::Value.c_str()), WrapSourceMap(*val_it));

            valuesCollectionArray->Set(j, valueObject);
        }

        parameterObject->Set(String::NewSymbol(snowcrash::SerializeKey::Values.c_str()), valuesCollectionArray);

        parametersCollectionArray->Set(i, parameterObject);
    }

    return parametersCollectionArray;
}

/** Wrap Payload source map */
static Local<Object> WrapPayload(const snowcrash::SourceMap<snowcrash::Payload>& payload)
{
    Local<Object> payloadObject = Object::New();

    // Name
    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), WrapSourceMap(payload.name));

    // Description
    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), WrapSourceMap(payload.description));

    // Headers
    Local<Object> headers = WrapSourceMaps(payload.headers.list);
    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Headers.c_str()), headers);

    // Body
    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Body.c_str()), WrapSourceMap(payload.body));

    // Schema
    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Schema.c_str()), WrapSourceMap(payload.schema));

    return payloadObject;
}

/** Wrap Transaction Examples source map */
static Local<Object> WrapTransactions(const snowcrash::SourceMap<snowcrash::TransactionExamples>& examples)
{
    Local<Object> transactionCollectionObject = Array::New(examples.list.size());

    size_t i = 0;

    for (snowcrash::Collection<snowcrash::SourceMap<snowcrash::TransactionExample> >::const_iterator it = examples.list.begin();
         it != examples.list.end();
         ++it, ++i) {

        Local<Object> transactionObject = Object::New();

        // Name
        transactionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), WrapSourceMap(it->name));

        // Description
        transactionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), WrapSourceMap(it->description));

        // Requests
        Local<Object> requests = Array::New(it->requests.list.size());
        size_t j = 0;

        for (snowcrash::Collection<snowcrash::SourceMap<snowcrash::Request> >::const_iterator request_it = it->requests.list.begin();
             request_it != it->requests.list.end();
             ++request_it, ++j) {

            requests->Set(j, WrapPayload(*request_it));
        }

        transactionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Requests.c_str()), requests);

        // Responses
        Local<Object> responses = Array::New(it->responses.list.size());
        j = 0;

        for (snowcrash::Collection<snowcrash::SourceMap<snowcrash::Response> >::const_iterator response_it = it->responses.list.begin();
             response_it != it->responses.list.end();
             ++response_it, ++j) {

            responses->Set(j, WrapPayload(*response_it));
        }

        transactionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Responses.c_str()), responses);

        // Insert transaction into collection
        transactionCollectionObject->Set(i, transactionObject);
    }

    return transactionCollectionObject;
}

/** Wrap Action source map */
static Local<Object> WrapAction(const snowcrash::SourceMap<snowcrash::Action>& action)
{
    Local<Object> actionObject = Object::New();

    // Name
    actionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), WrapSourceMap(action.name));

    // Description
    actionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), WrapSourceMap(action.description));

    // HTTP Request Method
    actionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Method.c_str()), WrapSourceMap(action.method));

    // Parameters
    Local<Object> parameters = WrapParameters(action.parameters);
    actionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Parameters.c_str()), parameters);

    // Transaction Examples
    Local<Object> examples = WrapTransactions(action.examples);
    actionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Examples.c_str()), examples);

    return actionObject;
}

/** Wrap snowcrash Resource source map */
static Local<Object> WrapResource(const snowcrash::SourceMap<snowcrash::Resource>& resource)
{
    Local<Object> resourceObject = Object::New();

    // Name
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), WrapSourceMap(resource.name));

    // Description
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), WrapSourceMap(resource.description));

    // URI template
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::URITemplate.c_str()), WrapSourceMap(resource.uriTemplate));

    // Model
    Local<Object> model = (resource.model.name.sourceMap.empty()) ? Object::New() : WrapPayload(resource.model);
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Model.c_str()), model);

    // Parameters
    Local<Object> parameters = WrapParameters(resource.parameters);
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Parameters.c_str()), parameters);

    // Actions
    Local<Object> actions = Array::New(resource.actions.list.size());
    size_t i = 0;

    for (snowcrash::Collection<snowcrash::SourceMap<snowcrash::Action> >::const_iterator it = resource.actions.list.begin();
         it != resource.actions.list.end();
         ++it, ++i) {

        actions->Set(i, WrapAction(*it));
    }

    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Actions.c_str()), actions);

    return resourceObject;
}

/** Wrap snowcrash Resource Group source map */
static Local<Object> WrapResourceGroup(const snowcrash::SourceMap<snowcrash::ResourceGroup>& group)
{
    Local<Object> groupObject = Object::New();

    // Name
    groupObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), WrapSourceMap(group.name));

    // Description
    groupObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), WrapSourceMap(group.description));

    // Resources
    Local<Object> resources = Array::New(group.resources.list.size());
    size_t i = 0;

    for (snowcrash::Collection<snowcrash::SourceMap<snowcrash::Resource> >::const_iterator it = group.resources.list.begin();
         it != group.resources.list.end();
         ++it, ++i) {

        resources->Set(i, WrapResource(*it));
    }

    groupObject->Set(String::NewSymbol(snowcrash::SerializeKey::Resources.c_str()), resources);

    return groupObject;
}

/** Wrap snowcrash Blueprint AST source map */
Local<Object> Sourcemap::WrapBlueprint(const snowcrash::SourceMap<snowcrash::Blueprint>& blueprint)
{
    Local<Object> blueprintWrap = constructor->NewInstance();

    // Metadata
    Local<Object> metadata = WrapSourceMaps(blueprint.metadata.list);
    blueprintWrap->Set(String::NewSymbol(snowcrash::SerializeKey::Metadata.c_str()), metadata);

    // Blueprint Name
    blueprintWrap->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), WrapSourceMap(blueprint.name));

    // Blueprint Description
    blueprintWrap->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), WrapSourceMap(blueprint.description));

    // Resource Groups
    Local<Object> groups = Array::New(blueprint.resourceGroups.list.size());
    size_t i = 0;

    for (snowcrash::Collection<snowcrash::SourceMap<snowcrash::ResourceGroup> >::const_iterator it = blueprint.resourceGroups.list.begin();
         it != blueprint.resourceGroups.list.end();
         ++it, ++i) {

        groups->Set(i, WrapResourceGroup(*it));
    }

    blueprintWrap->Set(String::NewSymbol(snowcrash::SerializeKey::ResourceGroups.c_str()), groups);

    return blueprintWrap;
}
