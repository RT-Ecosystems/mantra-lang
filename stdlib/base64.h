#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinBase64Encode(const std::vector<MantraValue>& args);
MantraValue builtinBase64Decode(const std::vector<MantraValue>& args);
}
