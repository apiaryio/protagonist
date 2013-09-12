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

/** Wrap Metadata */
static Local<Object> WrapMetadata(const snowcrash::Collection<snowcrash::Metadata>::type& metadataCollection) 
{
    Local<Object> metadataCollectionObject = Object::New();
    size_t i = 0;
    for (snowcrash::Collection<snowcrash::Metadata>::const_iterator it = metadataCollection.begin(); 
         it != metadataCollection.end();
         ++it, ++i) {

        Local<Object> metadataObject = Object::New();
        metadataObject->Set(String::NewSymbol(snowcrash::SerializeKey::Value.c_str()), String::New(it->second.c_str()));
        metadataCollectionObject->Set(String::NewSymbol(it->first.c_str()), metadataObject);
    }

    return metadataCollectionObject;
}

/** Wrap Parameters */
static Local<Object> WrapParameters(const snowcrash::Collection<snowcrash::Parameter>::type& parameters) 
{
    Local<Object> parametersCollectionObject = Object::New();
    for (snowcrash::Collection<snowcrash::Parameter>::const_iterator it = parameters.begin(); 
         it != parameters.end();
         ++it) {

        Local<Object> parameterObject = Object::New();

        // Description
        parameterObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), String::New(it->description.c_str()));

        // Type
        parameterObject->Set(String::NewSymbol(snowcrash::SerializeKey::Type.c_str()), String::New(it->type.c_str()));

        // Use flag
        parameterObject->Set(String::NewSymbol(snowcrash::SerializeKey::Required.c_str()), Boolean::New(it->use != snowcrash::OptionalParameterUse));        

        // Default value
        parameterObject->Set(String::NewSymbol(snowcrash::SerializeKey::Default.c_str()), String::New(it->defaultValue.c_str()));

        // Example value
        parameterObject->Set(String::NewSymbol(snowcrash::SerializeKey::Example.c_str()), String::New(it->exampleValue.c_str()));

        // Values
        Local<Object> valuesCollection = Array::New(it->values.size());
        size_t i = 0;
        for (snowcrash::Collection<snowcrash::Value>::const_iterator val_it = it->values.begin(); 
             val_it != it->values.end();
            ++val_it, ++i) {

            valuesCollection->Set(i, String::New(val_it->c_str()));
        } 
        parameterObject->Set(String::NewSymbol(snowcrash::SerializeKey::Values.c_str()), valuesCollection);


        parametersCollectionObject->Set(String::NewSymbol(it->name.c_str()), parameterObject);
    }

    return parametersCollectionObject;
}

/** Wrap Headers */
static Local<Object> WrapHeaders(const snowcrash::Collection<snowcrash::Header>::type& headers) 
{
    Local<Object> headersCollectionObject = Object::New();

    for (snowcrash::Collection<snowcrash::Header>::const_iterator it = headers.begin(); 
         it != headers.end();
         ++it) {

        Local<Object> headerObject = Object::New();
        headerObject->Set(String::NewSymbol(snowcrash::SerializeKey::Value.c_str()), String::New(it->second.c_str()));
        headersCollectionObject->Set(String::NewSymbol(it->first.c_str()), headerObject);
    }

    return headersCollectionObject;
}

/** Wrap Payload */
static Local<Object> WrapPayload(const snowcrash::Payload& payload) 
{
    Local<Object> payloadObject = Object::New();

    // Name
    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), String::New(payload.name.c_str()));

    // Description
    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), String::New(payload.description.c_str()));

    // Headers
    Local<Object> headers = WrapHeaders(payload.headers);
    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Headers.c_str()), headers);

    // Body
    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Body.c_str()), String::New(payload.body.c_str()));

    // Schema
    payloadObject->Set(String::NewSymbol(snowcrash::SerializeKey::Schema.c_str()), String::New(payload.schema.c_str()));    

    return payloadObject;
}

/** Wrap Transaction Examples */
static Local<Object> WrapTransactions(const snowcrash::Collection<snowcrash::TransactionExample>::type& examples) 
{
    Local<Object> transactionCollectionObject = Array::New(examples.size());

    size_t i = 0;
    for (snowcrash::Collection<snowcrash::TransactionExample>::const_iterator it = examples.begin(); 
         it != examples.end();
         ++it, ++i) {

        Local<Object> transactionObject = Object::New();

        // Name
        transactionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), String::New(it->name.c_str()));

        // Description
        transactionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), String::New(it->description.c_str()));

        // Requests
        Local<Object> requests = Array::New(it->requests.size());
        size_t j = 0;
        for (snowcrash::Collection<snowcrash::Request>::const_iterator request_it = it->requests.begin(); 
             request_it != it->requests.end();
             ++request_it, ++j) {

            requests->Set(j, WrapPayload(*request_it));
        }
        transactionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Requests.c_str()), requests);

        // Responses
        Local<Object> responses = Array::New(it->responses.size());
        j = 0;
        for (snowcrash::Collection<snowcrash::Response>::const_iterator response_it = it->responses.begin(); 
             response_it != it->responses.end();
             ++response_it, ++j) {

            responses->Set(j, WrapPayload(*response_it));
        }
        transactionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Responses.c_str()), responses);       

        // Insert transaction into collection
        transactionCollectionObject->Set(i, transactionObject);
    }    

    return transactionCollectionObject;
}

/** Wrap Action */
static Local<Object> WrapAction(const snowcrash::Action& action) 
{
    Local<Object> actionObject = Object::New();

    // Name
    actionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), String::New(action.name.c_str()));

    // Description
    actionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), String::New(action.description.c_str()));

    // HTTP Request Method
    actionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Method.c_str()), String::New(action.method.c_str()));

    // Parameters
    Local<Object> parameters = WrapParameters(action.parameters);
    actionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Parameters.c_str()), parameters); 

    // Headers
    Local<Object> headers = WrapHeaders(action.headers);
    actionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Headers.c_str()), headers);

    // Transaction Examples
    Local<Object> examples = WrapTransactions(action.examples);
    actionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Examples.c_str()), examples);

    return actionObject;
}

/** Wrap snowcrash Resource */
static Local<Object> WrapResource(const snowcrash::Resource& resource) 
{
    Local<Object> resourceObject = Object::New();

    // Name
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), String::New(resource.name.c_str()));

    // Description
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), String::New(resource.description.c_str()));

    // URI template
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::URITemplate.c_str()), String::New(resource.uriTemplate.c_str()));
    
    // Model
    Local<Object> model = (resource.model.name.empty()) ? Object::New() : WrapPayload(resource.model);
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Model.c_str()), model);

    // Parameters
    Local<Object> parameters = WrapParameters(resource.parameters);
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Parameters.c_str()), parameters); 

    // Headers
    Local<Object> headers = WrapHeaders(resource.headers);
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Headers.c_str()), headers);

    // Actions
    Local<Object> actions = Array::New(resource.actions.size());
    size_t i = 0;
    for (snowcrash::Collection<snowcrash::Action>::const_iterator it = resource.actions.begin(); 
         it != resource.actions.end();
         ++it, ++i) {

        actions->Set(i, WrapAction(*it));
    }
    resourceObject->Set(String::NewSymbol(snowcrash::SerializeKey::Actions.c_str()), actions);

    return resourceObject;
}

/** Wrap snowcrash Resource Group */
static Local<Object> WrapResourceGroup(const snowcrash::ResourceGroup& group) 
{
    Local<Object> groupObject = Object::New();

    // Name
    groupObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), String::New(group.name.c_str()));

    // Description
    groupObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()), String::New(group.description.c_str()));

    // Resources
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

/** Wrap snowcrash Blueprint AST */
Local<Object> Blueprint::WrapBlueprint(const snowcrash::Blueprint& blueprint)
{
    Local<Object> blueprintWrap = constructor->NewInstance();

    // Version 
    blueprintWrap->Set(String::NewSymbol(snowcrash::SerializeKey::ASTVersion.c_str()), String::New(AST_VERSION));

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
