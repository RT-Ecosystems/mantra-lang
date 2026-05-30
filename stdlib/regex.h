#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinRegexMatch(const std::vector<MantraValue>&);
MantraValue builtinRegexReplace(const std::vector<MantraValue>&);
}
