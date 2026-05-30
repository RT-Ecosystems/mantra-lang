#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinBase64Encode(const std::vector<MantraValue>&);
MantraValue builtinBase64Decode(const std::vector<MantraValue>&);
}
