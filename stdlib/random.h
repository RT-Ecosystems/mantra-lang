#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinRandom(const std::vector<MantraValue>&);
MantraValue builtinRandomInt(const std::vector<MantraValue>&);
MantraValue builtinChoice(const std::vector<MantraValue>&);
}
