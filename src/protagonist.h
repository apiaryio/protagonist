#ifndef PROTAGONIST_H
#define PROTAGONIST_H

#include <string>

#include <node.h>
#include <v8.h>
#include "nan.h"

namespace protagonist {

    using ErrorMessage = std::string;
    class Options;

    ErrorMessage ParseOptionsObject(Options&, v8::Local<v8::Object>, bool);

    //
    // Parse functions
    //
    extern NAN_METHOD(Parse);
    extern NAN_METHOD(ParseSync);

    //
    // Validate functions
    //
    extern NAN_METHOD(Validate);
    extern NAN_METHOD(ValidateSync);
}

#endif
