#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinJsonParse(const std::vector<MantraValue>& args);
MantraValue builtinJsonStringify(const std::vector<MantraValue>& args);
}
