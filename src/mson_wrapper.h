#ifndef MSON_WRAPPER_H
#define MSON_WRAPPER_H

#include <node.h>
#include <v8.h>
#include "MSON.h"

namespace protagonist {

    /** Wrap Named Type */
    v8::Local<v8::Value> WrapNamedType(const mson::NamedType& namedType);

    /** 
     * \brief Colletion item wrapper helpers 
     *
     * Designer's note: Ideally we should build a "wrap" facility in Snow Crash
     * based on templates instead of function overloading.
     */
    v8::Local<v8::Value> WrapCollectionItem(const mson::Value& value);
    v8::Local<v8::Value> WrapCollectionItem(const mson::Element& element);
    v8::Local<v8::Value> WrapCollectionItem(const mson::TypeSection& typeSection);
    v8::Local<v8::Value> WrapCollectionItem(const mson::TypeName& typeName);
    v8::Local<v8::Value> WrapCollectionItem(const std::string& text);

    /** Collection wrapper helper */
    template<typename T>
    v8::Local<v8::Value> WrapCollection(const T& collection) 
    {
        if (collection.empty())
            return v8::Local<v8::Value>::New(v8::Null());

        v8::Local<v8::Array> collectionArray = v8::Array::New(collection.size());

        size_t i = 0;
        for (typename T::const_iterator it = collection.begin();
             it != collection.end();
             ++it, ++i) {

            v8::Local<v8::Value> item = WrapCollectionItem(*it);
            collectionArray->Set(i, item);
        }

        return collectionArray;
    }
}

#endif
