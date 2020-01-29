#include "refractToV8.h"
#include <set>
#include "Serialize.h"  // Included because of serialize key for parseResult
#include "drafter.h"
#include "nan.h"
#include "refract/Element.h"
#include "refract/Exception.h"
#include "refract/FilterVisitor.h"  // Filtering just annotations
#include "refract/Iterate.h"
#include "refract/VisitorUtils.h"  // TypeQuery and GetValue<T>

using namespace v8;
using namespace refract;

struct v8Wrapper {
    bool sourcemap;
    Local<Value> v8_value;

    v8Wrapper() : sourcemap(false){};

    v8Wrapper(bool sourcemap) : sourcemap(sourcemap){};

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
    void operator()(const IElement& e){};
    void operator()(const RefElement& e);
    void operator()(const HolderElement& e);

    template <typename T>
    void operator()(const T& e)
    {
        static_assert(sizeof(T) == 0, "Unknown Element in v8Wrapper");
    }
};

const std::set<std::string> basic_elements = {
    "null", "string", "number", "boolean", "array", "object", "member"};

Local<String> v8_string(const std::string& value)
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

Local<Object> v8ElementCollection(const InfoElements& collection,
                                  bool sourcemap)
{
    Local<Object> result = Nan::New<Object>();

    for (const auto& el : collection) {
        Local<Value> key;
        Local<Value> value;

        if (!sourcemap) {
            if (el.first == "sourceMap") {
                continue;
            }
        }

        key = v8_string(el.first);

        if (el.second) {
            value = ElementToObject(el.second.get(), sourcemap);
        }

        result->Set(key, value);
    }

    return result;
}

Local<Object> v8Element(const IElement& e, bool sourcemap)
{
    Local<Object> res = Nan::New<Object>();
    res->Set(v8_string("element"), v8_string(e.element()));
    if (e.meta().size() > 0) {
        res->Set(v8_string("meta"), v8ElementCollection(e.meta(), sourcemap));
    }

    if (e.element() == "annotation") {
        sourcemap = true;
    }

    if (e.attributes().size() > 0) {
        Local<Object> attrs = v8ElementCollection(e.attributes(), sourcemap);
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

    if (!e.empty()) {
        size_t i = 0;
        Local<Array> array = Nan::New<Array>();

        for (const auto& el : e.get()) {
            array->Set(i, ElementToObject(el.get(), sourcemap));
            ++i;
        }

        obj->Set(v8_string("content"), array);
    }

    return obj;
}

Local<Value> v8RefElement(const RefElement& e, bool sourcemap)
{
    Local<Object> obj = v8Element(e, sourcemap);

    obj->Set(v8_string("content"), v8_string(e.get().symbol()));

    return obj;
}

void v8Wrapper::operator()(const NullElement& e)
{
    Local<Object> obj = v8Element(e, sourcemap);
    obj->Set(v8_string("content"), Nan::Null());
    v8_value = obj;
}

void v8Wrapper::operator()(const StringElement& e)
{
    Local<Object> obj = v8Element(e, sourcemap);
    if (!e.empty()) {
        obj->Set(v8_string("content"), v8_string(e.get().get()));
    }
    v8_value = obj;
}

void v8Wrapper::operator()(const NumberElement& e)
{
    Local<Object> obj = v8Element(e, sourcemap);
    if (!e.empty()) {
        Nan::JSON NanJSON;
        obj->Set(v8_string("content"),
                 NanJSON.Parse(v8_string(e.get().get())).ToLocalChecked());
    }
    v8_value = obj;
}

void v8Wrapper::operator()(const BooleanElement& e)
{
    Local<Object> obj = v8Element(e, sourcemap);
    if (!e.empty()) {
        obj->Set(v8_string("content"), Nan::New<Boolean>(e.get()));
    }
    v8_value = obj;
}

void v8Wrapper::operator()(const MemberElement& e)
{
    Local<Object> content = Nan::New<Object>();
    Local<Value> key;
    Local<Value> value;

    if (e.get().key()) {
        key = ElementToObject(e.get().key(), sourcemap);
    }

    if (e.get().value()) {
        value = ElementToObject(e.get().value(), sourcemap);
    }

    Local<Object> obj = v8Element(e, sourcemap);
    content->Set(v8_string("key"), key);
    content->Set(v8_string("value"), value);
    obj->Set(v8_string("content"), content);
    v8_value = obj;
}

void v8Wrapper::operator()(const ArrayElement& e)
{
    // XXX is GetValue appropriate here? shouldn't we just wrap drafter output?
    const auto* val = GetValue<const ArrayElement>{}(e);

    Local<Array> array = Nan::New<Array>();

    if (val && !val->empty()) {
        size_t i = 0;
        for (const auto& el : val->get()) {
            if (el) {
                array->Set(i, ElementToObject(el.get(), sourcemap));
            }
            ++i;
        }
    }

    Local<Object> res = v8Element(e, sourcemap);
    if (!e.empty()) {
        res->Set(v8_string("content"), array);
    }
    v8_value = res;
}

void v8Wrapper::operator()(const EnumElement& e)
{
    Local<Object> obj = v8Element(e, sourcemap);

    if (!e.empty()) {
        obj->Set(v8_string("content"),
                 ElementToObject(e.get().value(), sourcemap));
    }

    v8_value = obj;
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

void v8Wrapper::operator()(const RefElement& e)
{
    v8_value = v8RefElement(e, sourcemap);
}

void v8Wrapper::operator()(const HolderElement& e)
{
    Local<Object> obj = v8Element(e, sourcemap);

    if (!e.empty()) {
        obj->Set(v8_string("content"),
                 ElementToObject(e.get().data(), sourcemap));
    }

    v8_value = obj;
}

void v8Wrapper::operator()(const ObjectElement& e)
{
    Local<Object> obj = v8Element(e, sourcemap);

    if (!e.empty() && !e.get().empty()) {
        Local<Array> array = Nan::New<Array>();
        size_t i = 0;

        for (const auto& el : e.get()) {
            array->Set(i, ElementToObject(el.get(), sourcemap));
            ++i;
        }

        obj->Set(v8_string("content"), array);
    }

    v8_value = obj;
}

Local<Value> refract2v8(const IElement* res,
                        bool sourceMaps)
{
    assert(res);

    v8Wrapper f(sourceMaps);
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
             it != elements.end(); ++i, ++it) {
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
