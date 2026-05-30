#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinIniRead(const std::vector<MantraValue>&);
MantraValue builtinIniWrite(const std::vector<MantraValue>&);
}
