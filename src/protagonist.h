#ifndef PROTAGONIST_H
#define PROTAGONIST_H

#include <node.h>
#include <v8.h>
#include "nan.h"
#include "drafter.h"

namespace protagonist {

    //
    // Options parsing
    ///
    struct OptionsResult {
      drafter_parse_options parseOptions;
      drafter_serialize_options serializeOptions;
      char *error;
    };

    OptionsResult* ParseOptionsObject(v8::Local<v8::Object>, bool);
    void FreeOptionsResult(OptionsResult** optionsResult);

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
