#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinRandomInt(const std::vector<MantraValue>&);
MantraValue builtinChoice(const std::vector<MantraValue>&);
}
