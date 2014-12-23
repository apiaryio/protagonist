#include "mson_wrapper.h"
#include "Serialize.h"

using namespace v8;
using namespace protagonist;

// TODO: Move to Snow Crash
static const std::string NameKey = "name";
static const std::string LiteralKey = "literal";
static const std::string VariableKey = "variable";
static const std::string TypeSpecificationKey = "typeSpecification";
static const std::string AttributesKey = "attributes";
static const std::string SectionsKey = "sections";
static const std::string ClassKey = "class";
static const std::string ContentKey = "content";
static const std::string ValueDefinitionKey = "valueDefinition";
static const std::string TypeDefinitionKey = "typeDefinition";
static const std::string NestedTypesKey = "nestedTypes";

// Forward declarations
static Local<Value> WrapTypeDefinition(const mson::TypeDefinition& typeDefinition);

// Wrap Symbol
static Local<Value> WrapSymbol(const mson::Symbol& symbol)
{
    Local<Object> symbolObject = Object::New();

    // Literal
    symbolObject->Set(String::NewSymbol(LiteralKey.c_str()), String::New(symbol.literal.c_str()));

    // Variable
    symbolObject->Set(String::NewSymbol(VariableKey.c_str()), Boolean::New(symbol.variable));

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
    if (typeName.empty())
        return Local<Value>::New(Null());

    if (typeName.base != mson::UndefinedTypeName) {
        // Base type
        return WrapBaseTypeName(typeName.base);
    }
    else {
        // Symbol
        return WrapSymbol(typeName.symbol);
    }
}

// Wrap Type Name alias
v8::Local<v8::Value> protagonist::WrapCollectionItem(const mson::TypeName& typeName)
{
    return WrapTypeName(typeName);
}

// Wrap Type Specification
static Local<Value> WrapTypeSpecification(const mson::TypeSpecification& typeSpecification)
{
    Local<Object> typeSpecificationObject = Object::New();

    // Name
    typeSpecificationObject->Set(String::NewSymbol(NameKey.c_str()), WrapTypeName(typeSpecification.name));

    // Nested Types
    if (!typeSpecification.nestedTypes.empty()) {
        Local<Value> v = WrapCollection<mson::TypeNames>(typeSpecification.nestedTypes);
        typeSpecificationObject->Set(String::NewSymbol(NestedTypesKey.c_str()), v);
    }

    return typeSpecificationObject;
}

// Generic string wrapper
v8::Local<v8::Value> protagonist::WrapCollectionItem(const std::string& text)
{
    return String::NewSymbol(text.c_str());
}

// Wrap Attributes
static Local<Value> WrapTypeAttributes(const mson::TypeAttributes& typeAttributes)
{
    typedef std::vector<std::string> TypeAttributesNames;
    TypeAttributesNames attributesNames;

    if (typeAttributes & mson::RequiredTypeAttribute)
        attributesNames.push_back("required");

    if (typeAttributes & mson::OptionalTypeAttribute)
        attributesNames.push_back("optional");

    if (typeAttributes & mson::FixedTypeAttribute)
        attributesNames.push_back("fixed");

    if (typeAttributes & mson::SampleTypeAttribute)
        attributesNames.push_back("sample");

    if (typeAttributes & mson::DefaultTypeAttribute)
        attributesNames.push_back("default");

    if (attributesNames.empty())
        return Local<Value>::New(Null());

    return WrapCollection<TypeAttributesNames>(attributesNames);
}

// Wrap Type Definition
static Local<Value> WrapTypeDefinition(const mson::TypeDefinition& typeDefinition)
{
    if (typeDefinition.empty())
        return Local<Value>::New(Null());

    Local<Object> typeDefinitionObject = Object::New();

    // Type Specification
    typeDefinitionObject->Set(String::NewSymbol(TypeSpecificationKey.c_str()), 
                                                WrapTypeSpecification(typeDefinition.typeSpecification));

    // Attributes
    if (typeDefinition.attributes != 0) {
        Local<Value> v = WrapTypeAttributes(typeDefinition.attributes);
        typeDefinitionObject->Set(String::NewSymbol(AttributesKey.c_str()), v);
    }

    return typeDefinitionObject;
}

// Wrap Type Section Class
static Local<Value> WrapTypeSectionClass(const mson::TypeSection::Class& typeSectionClass)
{
    switch (typeSectionClass) {

        case mson::TypeSection::BlockDescriptionClass:
            return String::NewSymbol("blockDescription");

        case mson::TypeSection::MemberTypeClass:
            return String::NewSymbol("memberType");

        case mson::TypeSection::SampleClass:
            return String::NewSymbol("sample");

        case mson::TypeSection::DefaultClass:
            return String::NewSymbol("default");

        default:
            return Local<Value>::New(Null());
    }
}

// Wrap Element Class
static Local<Value> WrapElementClass(const mson::Element::Class& elementClass)
{
    switch (elementClass) {

        case mson::Element::PropertyClass:
            return String::NewSymbol("property");

        case mson::Element::ValueClass:
            return String::NewSymbol("value");

        case mson::Element::MixinClass:
            return String::NewSymbol("mixin");

        case mson::Element::OneOfClass:
            return String::NewSymbol("oneOf");

        case mson::Element::GroupClass:
            return String::NewSymbol("group");

        default:
            return Local<Value>::New(Null());
    }
}

// Value 
Local<Value> protagonist::WrapCollectionItem(const mson::Value& value) 
{
    Local<Object> valueObject = Object::New();

    // Literal
    valueObject->Set(String::NewSymbol(LiteralKey.c_str()), String::New(value.literal.c_str()));

    // Variable
    valueObject->Set(String::NewSymbol(VariableKey.c_str()), Boolean::New(value.variable));

    return valueObject;
}

// Value Definition
static Local<Value> WrapValueDefinition(const mson::ValueDefinition& valueDefinition) 
{
    Local<Object> valueDefinitionObject = Object::New();

    // Values
    if (!valueDefinition.values.empty()) {
        Local<Value> v = WrapCollection<mson::Values>(valueDefinition.values);
        valueDefinitionObject->Set(String::NewSymbol(snowcrash::SerializeKey::Values.c_str()), v);
    }

    // Type Definition
    if (!valueDefinition.typeDefinition.empty()) {
        Local<Value> v = WrapTypeDefinition(valueDefinition.typeDefinition);
        valueDefinitionObject->Set(String::NewSymbol(TypeDefinitionKey.c_str()), v);
    }

    return valueDefinitionObject;
}

// Value Member
static Local<Object> WrapValueMember(const mson::ValueMember& valueMember, bool reservePropertyName = false)
{
    Local<Object> valueMemberObject = Object::New();

    // Due to limitations of v8 API reserve property name upfront
    if (reservePropertyName) {
        valueMemberObject->Set(String::NewSymbol(NameKey.c_str()), Null());
    }

    // Description
    if (!valueMember.description.empty()) {
        valueMemberObject->Set(String::NewSymbol(snowcrash::SerializeKey::Description.c_str()),
                               String::NewSymbol(valueMember.description.c_str()));
    }

    // Value definition
    if (!valueMember.valueDefinition.empty()) {
        Local<Value> v = WrapValueDefinition(valueMember.valueDefinition);
        valueMemberObject->Set(String::NewSymbol(ValueDefinitionKey.c_str()), v);
    }
    
    // Sections
    if (!valueMember.sections.empty()) {
        Local<Value> v = WrapCollection<mson::TypeSections>(valueMember.sections);
        valueMemberObject->Set(String::NewSymbol(SectionsKey.c_str()), v);
    }

    return valueMemberObject;
}

// Property Name
static Local<Value> WrapPropertyName(const mson::PropertyName& propertyName) 
{
    Local<Object> propertyNameObject = Object::New();
    
    if (!propertyName.literal.empty()) {
        // Literal
        propertyNameObject->Set(String::NewSymbol(LiteralKey.c_str()), String::New(propertyName.literal.c_str()));
    }
    else {
        // Variable 
        Local<Value> v = WrapValueDefinition(propertyName.variable);
        propertyNameObject->Set(String::NewSymbol(VariableKey.c_str()), v);
    }

    return propertyNameObject;
}

// Property Member
static Local<Value> WrapPropertyMember(const mson::PropertyMember& propertyMember)
{
    Local<Object> propertyMemberObject = WrapValueMember(propertyMember, !propertyMember.name.empty());

    // Name
    Local<Value> v = WrapPropertyName(propertyMember.name);
    propertyMemberObject->Set(String::NewSymbol(NameKey.c_str()), v);

    return propertyMemberObject;
}

// Elements / One Of 
static Local<Value> WrapElements(const mson::Elements& elements)
{
    Local<Value> v = WrapCollection<mson::Elements>(elements);
    return v;
}

// Wrap Element Content
static Local<Value> WrapElementContent(const mson::Element::Content& elementContent,
                                       const mson::Element::Class& elementClass)
{
    switch (elementClass) {

        case mson::Element::PropertyClass:
        {
            return WrapPropertyMember(elementContent.property);
        }

        case mson::Element::ValueClass:
        {
            return WrapValueMember(elementContent.value);
        }

        case mson::Element::MixinClass:
        {
            return WrapTypeDefinition(elementContent.mixin);
        }

        case mson::Element::OneOfClass:
        {
            return WrapElements(elementContent.elements());
        }

        case mson::Element::GroupClass:
        {
            return WrapElements(elementContent.elements());
        }

        default:
            return Local<Value>::New(Null());
    }
}

// Wrap Element
Local<Value> protagonist::WrapCollectionItem(const mson::Element& element)
{
    Local<Object> elementObject = Object::New();

    // Class
    elementObject->Set(String::NewSymbol(ClassKey.c_str()), WrapElementClass(element.klass));

    // Content
    elementObject->Set(String::NewSymbol(ContentKey.c_str()), WrapElementContent(element.content,
                                                                                 element.klass));

    return elementObject;
}

// Wrap Type Section Content
static Local<Value> WrapTypeSectionContentValue(const mson::TypeSection::Content& typeSectionContent,
                                                const mson::TypeSection::Class& typeSectionClass,
                                                const mson::BaseType& baseType)
{
    switch (typeSectionClass) {
        case mson::TypeSection::BlockDescriptionClass:
        {
            Local<String> v = String::New(typeSectionContent.description.c_str());
            return v;
        }

        case mson::TypeSection::SampleClass:
        case mson::TypeSection::DefaultClass:
        {
            if (baseType == mson::PrimitiveBaseType ||
                baseType == mson::ImplicitPrimitiveBaseType) {

                Local<String> v = String::New(typeSectionContent.value.c_str());
                return v;
            }
            else {
                return WrapCollection<mson::Elements>(typeSectionContent.elements());
            }
        }

        case mson::TypeSection::MemberTypeClass:
        {
            return WrapCollection<mson::Elements>(typeSectionContent.elements());
        }

        default:
        return Local<Value>::New(Null());
    }
}

// Wrap Type Section
Local<Value> protagonist::WrapCollectionItem(const mson::TypeSection& typeSection)
{
    Local<Object> typeSectionObject = Object::New();

    // Class
    typeSectionObject->Set(String::NewSymbol(ClassKey.c_str()), WrapTypeSectionClass(typeSection.klass));

    // Content
    typeSectionObject->Set(String::NewSymbol(ContentKey.c_str()), WrapTypeSectionContentValue(typeSection.content,
                                                                                              typeSection.klass,
                                                                                              typeSection.baseType));
    return typeSectionObject;
}

// Wrap Named Type
Local<Value> protagonist::WrapNamedType(const mson::NamedType& namedType)
{
    if (namedType.empty())
        return Local<Value>::New(Null());

    Local<Object> typeObject = Object::New();

    // Name
    if (!namedType.name.empty()) {
        Local<Value> v = WrapTypeName(namedType.name);
        typeObject->Set(String::NewSymbol(NameKey.c_str()), v);
    }

    // Ancestor type definition
    if (!namedType.typeDefinition.empty()) {
        Local<Value> v = WrapTypeDefinition(namedType.typeDefinition);
        typeObject->Set(String::NewSymbol(TypeDefinitionKey.c_str()), v);
    }

    // Type sections
    if (!namedType.sections.empty()) {
        Local<Value> v = WrapCollection<mson::TypeSections>(namedType.sections);
        typeObject->Set(String::NewSymbol(SectionsKey.c_str()), v);
    }

    return typeObject;
}
