#include "serial.h"
#include <cstdio>
#include <cstdlib>
#include <string>
namespace mantra::stdlib {
MantraValue builtinSerialOpen(const std::vector<MantraValue>& a){
    return MantraValue::boolean(true);
}
MantraValue builtinSerialRead(const std::vector<MantraValue>& a){
    return MantraValue::string("[serial data]");
}
MantraValue builtinSerialWrite(const std::vector<MantraValue>& a){
    return MantraValue::boolean(true);
}
MantraValue builtinSerialClose(const std::vector<MantraValue>&){
    return MantraValue::boolean(true);
}
}
