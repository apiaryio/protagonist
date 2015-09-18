#include "v8_wrapper.h"

using namespace v8;
using namespace protagonist;

// Forward declarations
static Local<Value> v8_wrap_null();
static Local<Value> v8_wrap_string(const std::string& value);
static Local<Value> v8_wrap_number(double value);
static Local<Value> v8_wrap_boolean(bool value);
static Local<Value> v8_wrap_array(const sos::Base& value);
static Local<Value> v8_wrap_object(const sos::Base& value);

// Wrap interface
Local<Value> protagonist::v8_wrap(const sos::Base& base)
{
    switch (base.type) {
        case sos::Base::StringType:
            return v8_wrap_string(base.str);

        case sos::Base::NumberType:
            return v8_wrap_number(base.number);

        case sos::Base::BooleanType:
            return v8_wrap_boolean(base.boolean);

        case sos::Base::ArrayType:
            return v8_wrap_array(base);

        case sos::Base::ObjectType:
            return v8_wrap_object(base);

        case sos::Base::NullType:
        default:
            return v8_wrap_null();
    }
}

Local<Value> v8_wrap_null()
{
    return Nan::Null();
}

Local<Value> v8_wrap_string(const std::string& value)
{
    return Nan::New<String>(value.c_str()).ToLocalChecked();
}

Local<Value> v8_wrap_number(double value)
{
    return Nan::New<Number>(value);
}

Local<Value> v8_wrap_boolean(bool value)
{
    return Nan::New<Boolean>(value);
}

Local<Value> v8_wrap_array(const sos::Base& value)
{
    Local<Array> wrappedArray = Nan::New<Array>();
    size_t i = 0;

    for (sos::Bases::const_iterator it = value.array().begin();
        it != value.array().end();
        ++i, ++it) {

        Local<Value> arrayElement = v8_wrap(*it);
        wrappedArray->Set(i, arrayElement);
    }

    return wrappedArray;
}

Local<Value> v8_wrap_object(const sos::Base& value)
{
    Local<Object> wrappedObject = Nan::New<Object>();
    size_t i = 0;

    for (sos::Keys::const_iterator it = value.keys.begin();
        it != value.keys.end();
        ++i, ++it) {

        Local<Value> propertyValue = v8_wrap(value.object().at(*it));
        wrappedObject->Set(v8_wrap_string(*it), propertyValue);
    }

    return wrappedObject;
}
