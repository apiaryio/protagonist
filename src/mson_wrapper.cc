#include "mson_wrapper.h"

// TODO: Remove
// #include <iostream>

using namespace v8;
using namespace protagonist;

// TODO: Move to Snow Crash
static const std::string Name = "name";
static const std::string Literal = "literal";
static const std::string Variable = "variable";
static const std::string Base = "base";
static const std::string TypeSpecification = "typeSpecification";
static const std::string Attributes = "attributes";
static const std::string Sections = "sections";

// Wrap Symbol
static Local<Value> WrapSymbol(const mson::Symbol& symbol)
{
    Local<Object> symbolObject = Object::New();

    // Literal
    symbolObject->Set(String::NewSymbol(Literal.c_str()), String::New(symbol.literal.c_str()));

    // Variable
    symbolObject->Set(String::NewSymbol(Variable.c_str()), Boolean::New(symbol.variable));

    return symbolObject;
}

// Wrap Base Type Name
static Local<Value> WrapBaseTypeName(const mson::BaseTypeName& baseTypeName)
{
    switch (baseTypeName) {

        case mson::BooleanTypeName:
            return String::NewSymbol("boolean");

        case mson::StringTypeName:
            return String::NewSymbol("string");

        case mson::NumberTypeName:
            return String::NewSymbol("number");

        case mson::ArrayTypeName:
            return String::NewSymbol("array");

        case mson::EnumTypeName:
            return String::NewSymbol("enum");

        case mson::ObjectTypeName:
            return String::NewSymbol("object");

        case mson::UndefinedTypeName:
        default:
            return Local<Value>::New(Null());
    }
}

// Wrap Type Name
static Local<Value> WrapTypeName(const mson::TypeName& typeName)
{
    Local<Object> typeNameObject = Object::New();

    if (typeName.empty())
        return Local<Value>::New(Null());

    if (typeName.name != mson::UndefinedTypeName) {
        // Base type
        typeNameObject->Set(String::NewSymbol(Name.c_str()), WrapBaseTypeName(typeName.name));
    }
    else {
        // Symbol
        return WrapSymbol(typeName.symbol);
    }

    return typeNameObject;
}

// Wrap Type Specification
static Local<Value> WrapTypeSpecification(const mson::TypeSpecification& typeSpecification)
{
    Local<Object> typeSpecificationObject = Object::New();

    // Name
    typeSpecificationObject->Set(String::NewSymbol(Name.c_str()), WrapTypeName(typeSpecification.name));

    // Nested Types
    // TODO:

    return typeSpecificationObject;
}

// Wrap Attributes

// Wrap Type Definition
static Local<Value> WrapTypeDefinition(const mson::TypeDefinition& typeDefinition)
{
    Local<Object> typeDefinitionObject = Object::New();

    if (typeDefinition.empty())
        return Local<Value>::New(Null());

    // Type Specification
    typeDefinitionObject->Set(String::NewSymbol(TypeSpecification.c_str()), WrapTypeSpecification(typeDefinition.typeSpecification));

    // Attributes
    // TODO:

    return typeDefinitionObject;
}

// Wrap Type Sections
static Local<Value> WrapTypeSections(const mson::TypeSections& typeSections)
{
    // TODO: Remove
    //std::cout << "typeSections count:" << typeSections.size() << "\n\n";

    if (typeSections.empty())
        return Local<Value>::New(Null());

    Local<Array> typeSectionsArray = Array::New(typeSections.size());

    size_t i = 0;
    for (mson::TypeSections::const_iterator it = typeSections.begin();
        it != typeSections.end();
        ++it, ++i) {

            Local<Object> section = Object::New();



            // // Name
            // metadataObject->Set(String::NewSymbol(snowcrash::SerializeKey::Name.c_str()), String::New(it->first.c_str()));
            //
            // // Value
            // metadataObject->Set(String::NewSymbol(snowcrash::SerializeKey::Value.c_str()), String::New(it->second.c_str()));

            typeSectionsArray->Set(i, section);
        }

    return typeSectionsArray;
}

// Wrap Named Type
Local<Value> protagonist::WrapNamedType(const mson::NamedType& namedType)
{
    if (namedType.empty())
        return Local<Value>::New(Null());

    Local<Object> typeObject = Object::New();

    // Name
    Local<Value> v = WrapTypeName(namedType.name);
    if (v != Null())
        typeObject->Set(String::NewSymbol(Name.c_str()), v);

    // Ancestor type definition
    v = WrapTypeDefinition(namedType.base);
    if (v != Null())
        typeObject->Set(String::NewSymbol(Base.c_str()), v);

    // Type sections
    v = WrapTypeSections(namedType.sections);
    if (v != Null())
        typeObject->Set(String::NewSymbol(Sections.c_str()), v);

    return typeObject;
}
