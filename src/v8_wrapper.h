#ifndef PROTAGONIST_V8_WRAPPER_H
#define PROTAGONIST_V8_WRAPPER_H

#include <node.h>
#include <v8.h>
#include "nan.h"
#include "sos.h"

namespace protagonist {

    // Simple object serialization wrapper for V8
    v8::Local<v8::Value> v8_wrap(const sos::Base& base);
}

#endif
