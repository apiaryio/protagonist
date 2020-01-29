#ifndef REFRACT2V8_H
#define REFRACT2V8_H

#include <node.h>
#include <v8.h>
#include <refract/ElementIfc.h>
#include <drafter.h>

v8::Local<v8::Value> refract2v8(const refract::IElement* res, bool sourceMaps);
v8::Local<v8::Value> annotations2v8(const refract::IElement* res);

#endif
