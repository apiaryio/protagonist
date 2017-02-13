#include <set>
#include "drafter.h"
#include "nan.h"
#include "refract2v8.h"
#include "refract/Element.h"
#include "refract/VisitorUtils.h" // TypeQuery and GetValue<T>
#include "refract/FilterVisitor.h" // Filtering just annotations
#include "refract/Iterate.h"
#include "Serialize.h" // Included because of serialize key for parseResult

using namespace v8;
using namespace refract;

struct v8Wrapper {

    bool sourcemap;
    Local<Value> v8_value;

    v8Wrapper(): sourcemap(false){};

    v8Wrapper(bool sourcemap): sourcemap(sourcemap){};

    void operator()(const NullElement& e);
    void operator()(const StringElement& e);
    void operator()(const NumberElement& e);
    void operator()(const BooleanElement& e);
    void operator()(const MemberElement& e);
    void operator()(const ArrayElement& e);
    void operator()(const EnumElement& e);
    void operator()(const ExtendElement& e);
    void operator()(const OptionElement& e);
    void operator()(const SelectElement& e);
    void operator()(const ObjectElement& e);
    void operator()(const IElement& e) {};
    template<typename T>
    void operator()(const T& e) {
        static_assert(sizeof(T) == 0, "Unknown Element in v8Wrapper");
    }
};

const std::set<std::string> basic_elements = {
    "null",
    "string",
    "number",
    "boolean",
    "array",
    "object",
    "member"
};

struct IsFullRender {
    typedef const IElement* first_argument_type;
    typedef const bool second_argument_type;
    typedef const bool result_type;

    bool operator()(const IElement* e, const bool sourcemap, const bool nonbasic = true) const {

        if (basic_elements.find(e->element()) == basic_elements.end() && nonbasic) return true;

        IElement::MemberElementCollection::const_iterator it = e->attributes.find("sourceMap");
        // is there sourceMap in attributes
        if (it != e->attributes.end()) {
            // don't count sourceMap as attribute
            return sourcemap ? true : e->meta.size() || (e->attributes.size() - 1);
        }
        return e->meta.size() || e->attributes.size();
    }
};


Local<Value> v8_string(const std::string& value)
{
    return Nan::New<String>(value.c_str()).ToLocalChecked();
}


Local<Value> ElementToObject(const IElement* e, bool sourcemap)
{
    v8Wrapper f(sourcemap);
    Visitor v(f);
    e->content(v);
    return f.v8_value;
}


Local<Object> v8ElementCollection(const IElement::MemberElementCollection& collection, bool sourcemap)
{
    typedef IElement::MemberElementCollection::const_iterator iterator;

    Local<Object> result = Nan::New<Object>();

    for (iterator it = collection.begin(); it != collection.end(); ++it) {

        Local<Value> key;
        Local<Value> value;

        if (!sourcemap) {
            StringElement* str = TypeQueryVisitor::as<StringElement>((*it)->value.first);
            if (str && str->value == "sourceMap"){
                continue;
            }
        }

        if ((*it)->value.first) {
            key = ElementToObject((*it)->value.first, sourcemap);
        }

        if ((*it)->value.second) {
            value = ElementToObject((*it)->value.second, sourcemap);
        }

        result->Set(key, value);
    }

    return result;
}


Local<Object> v8Element(const IElement& e, bool sourcemap)
{
    Local<Object> res = Nan::New<Object>();
    res->Set(v8_string("element"), v8_string(e.element()));
    if (e.meta.size() > 0) {
        res->Set(v8_string("meta"), v8ElementCollection(e.meta, sourcemap));
    }

    if (e.element() == "annotation") {
        sourcemap = true;
    }

    if (e.attributes.size() > 0) {
        Local<Object> attrs = v8ElementCollection(e.attributes, sourcemap);
        MaybeLocal<Array> maybeProps = Nan::GetOwnPropertyNames(attrs);
        if (!maybeProps.IsEmpty()) {
            Local<Array> props = maybeProps.ToLocalChecked();
            if (props->Length() > 0) {
                res->Set(v8_string("attributes"), attrs);
            }
        }
    }
    return res;
}

template <typename T>
Local<Object> v8ValueList(const T& e, bool sourcemap)
{
    Local<Object> obj = v8Element(e, sourcemap);

    if (!e.value.empty()) {
        size_t i = 0;
        Local<Array> array = Nan::New<Array>();
        typedef typename T::ValueType::const_iterator iterator;

        for (iterator it = e.value.begin(); it != e.value.end(); ++i, ++it) {
            array->Set(i, ElementToObject(*it, sourcemap));
        }
        obj->Set(v8_string("content"), array);
    }

    return obj;
}


Local<Value> v8RefElement(const ObjectElement& e, bool sourcemap)
{
    typedef ObjectElement::ValueType::const_iterator iterator;
    Local<Object> obj = v8Element(e, sourcemap);

    if(IsFullRender()(&e, sourcemap, false)) {
        Local<Array> array = Nan::New<Array>();
        size_t i = 0;

        for (iterator it = e.value.begin(); it != e.value.end(); ++i, ++it) {
            array->Set(i, ElementToObject(*it, sourcemap));
        }
        obj->Set(v8_string("content"), array);
    }
    else {
        Local<Object> content = Nan::New<Object>();
        Local<Value> key;
        Local<Value> value;
        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {
            MemberElement *m = dynamic_cast<MemberElement*>(*it);
            if (m->value.first) {
                key = ElementToObject(m->value.first, sourcemap);
            }

            if (m->value.second) {
                value = ElementToObject(m->value.second, sourcemap);
            }

            content->Set(key, value);
        }
        obj->Set(v8_string("content"), content);
    }

    return obj;
}

void v8Wrapper::operator()(const NullElement& e)
{
    if (IsFullRender()(&e, sourcemap)) {
        Local<Object> obj = v8Element(e, sourcemap);
        obj->Set(v8_string("content"), Nan::Null());
        v8_value = obj;
    }
    else {
        v8_value =  Nan::Null();
    }
}

void v8Wrapper::operator()(const StringElement& e)
{
    if (IsFullRender()(&e, sourcemap)) {
        Local<Object> obj = v8Element(e, sourcemap);
        obj->Set(v8_string("content"), Nan::New<String>(e.value).ToLocalChecked());
        v8_value = obj;
    }
    else {
        v8_value = Nan::New<String>(e.value).ToLocalChecked();
    }
}

void v8Wrapper::operator()(const NumberElement& e)
{
    if (IsFullRender()(&e, sourcemap)) {
        Local<Object> obj = v8Element(e, sourcemap);
        if (e.value) {
            obj->Set(v8_string("content"), Nan::New<Number>(e.value));
        }
        v8_value = obj;
    }
    else {
        v8_value =  Nan::New<Number>(e.value);
    }
}

void v8Wrapper::operator()(const BooleanElement& e)
{
    if (IsFullRender()(&e, sourcemap)) {
        Local<Object> obj = v8Element(e, sourcemap);
        obj->Set(v8_string("content"), Nan::New<Boolean>(e.value));
        v8_value = obj;
    }
    else {
        v8_value =  Nan::New<Boolean>(e.value);
    }
}

void v8Wrapper::operator()(const MemberElement& e)
{
    Local<Object> content = Nan::New<Object>();
    Local<Value> key;
    Local<Value> value;

    if (e.value.first) {
        key = ElementToObject(e.value.first, sourcemap);
    }

    if (e.value.second) {
        value = ElementToObject(e.value.second, sourcemap);
    }

    Local<Object> obj = v8Element(e, sourcemap);
    content->Set(v8_string("key"), key);
    content->Set(v8_string("value"), value);
    obj->Set(v8_string("content"), content);
    v8_value = obj;
}

void v8Wrapper::operator()(const ArrayElement& e)
{
    const ArrayElement::ValueType* val = GetValue<ArrayElement>(e);

    Local<Array> array = Nan::New<Array>();

    size_t i = 0;

    for (ArrayElement::ValueType::const_iterator it = val->begin();
         it != val->end();
         ++i, ++it) {
        if (*it) {
            array->Set(i, ElementToObject(*it, sourcemap));
        }
    }
    if (IsFullRender()(&e, sourcemap)) {
        Local<Object> res = v8Element(e, sourcemap);
        res->Set(v8_string("content"), array);
        v8_value = res;
    }
    else {
        v8_value = array;
    }
}

void v8Wrapper::operator()(const EnumElement& e)
{
    v8_value = v8ValueList(e, sourcemap);
}

void v8Wrapper::operator()(const ExtendElement& e)
{
    throw NotImplemented("ExtendElement serialization Not Implemented");
}

void v8Wrapper::operator()(const OptionElement& e)
{
    v8_value = v8ValueList(e, sourcemap);
}

void v8Wrapper::operator()(const SelectElement& e)
{
    v8_value = v8ValueList(e, sourcemap);
}

void v8Wrapper::operator()(const ObjectElement& e)
{
    typedef ObjectElement::ValueType::const_iterator iterator;

    // RefElement is not yet implemented in refract so we treat it as
    // a special ObjectElement.
    if (e.element() == "ref") {
        v8_value = v8RefElement(e, sourcemap);
        return;
    }

    Local<Object> obj = v8Element(e, sourcemap);

    if(IsFullRender()(&e, sourcemap) && !e.value.empty()) {

        Local<Array> array = Nan::New<Array>();
        size_t i = 0;

        for (iterator it = e.value.begin(); it != e.value.end(); ++i, ++it) {
            array->Set(i, ElementToObject(*it, sourcemap));
        }

        obj->Set(v8_string("content"), array);
    }
    else {

        for (iterator it = e.value.begin(); it != e.value.end(); ++it) {

            Local<Value> key;
            Local<Value> value;
            MemberElement *m = dynamic_cast<MemberElement*>(*it);

            if (m->value.first) {
                key = ElementToObject(m->value.first, sourcemap);
            }

            if (m->value.second) {
                value = ElementToObject(m->value.second, sourcemap);
            }

            obj->Set(key, value);
        }
    }

    v8_value = obj;
}

Local<Value> refract2v8(const IElement* res,
                        drafter_serialize_options serializeOptions)
{
    assert(res);

    v8Wrapper f(serializeOptions.sourcemap);
    Visitor v(f);
    res->content(v);

    return f.v8_value;

}

Local<Value> annotations2v8(const IElement* res)
{
    FilterVisitor filter(query::Element("annotation"));
    Iterate<Children> iterate(filter);

    iterate(*res);

    if (!filter.empty()) {

        std::vector<const IElement*> elements = filter.elements();
        Local<Array> array = Nan::New<Array>();
        size_t i = 0;

        for (std::vector<const IElement*>::const_iterator it = elements.begin();
             it != elements.end();
             ++i, ++it) {

            if (*it) {
                array->Set(i, ElementToObject(*it, true));
            }
        }

        Local<Object> annotations = Nan::New<Object>();
        annotations->Set(v8_string("element"),
                         v8_string(drafter::SerializeKey::ParseResult));
        annotations->Set(v8_string("content"), array);
        return annotations;
    }

    return Nan::Null();
}
