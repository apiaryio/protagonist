#ifndef MSON_WRAPPER_H
#define MSON_WRAPPER_H

#include <node.h>
#include <v8.h>
#include "MSON.h"

namespace protagonist {

    /** Wrap Named Type */
    v8::Local<v8::Object> WrapNamedType(const mson::NamedType& namedType);
}

#endif
