#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION 1
#endif  // BUILDING_NODE_EXTENSION

#include <node.h>
#include "connection.h"

using namespace v8;

void InitAll(Handle<Object> target) {
	Connection::Init(target);
}

NODE_MODULE(pgsync, InitAll)

