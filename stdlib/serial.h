#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinSerialOpen(const std::vector<MantraValue>&);
MantraValue builtinSerialRead(const std::vector<MantraValue>&);
MantraValue builtinSerialWrite(const std::vector<MantraValue>&);
MantraValue builtinSerialClose(const std::vector<MantraValue>&);
}
