#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinHttpGet(const std::vector<MantraValue>& args);
MantraValue builtinHttpPost(const std::vector<MantraValue>& args);
}
