#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinRegexMatch(const std::vector<MantraValue>& args);
MantraValue builtinRegexReplace(const std::vector<MantraValue>& args);
}
