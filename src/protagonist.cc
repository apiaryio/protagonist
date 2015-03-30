#include "protagonist.h"

using namespace v8;
using namespace protagonist;

void Init(Handle<Object> exports) {

    // SourceAnnotation object
    SourceAnnotation::Init(exports);

    // Result object
    Result::Init(exports);

    // Parse function
    exports->Set(NanNew<String>("parse"), NanNew<FunctionTemplate>(Parse)->GetFunction());
}

NODE_MODULE(protagonist, Init)
