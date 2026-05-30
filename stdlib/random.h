#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinRandom(const std::vector<MantraValue>& args);
MantraValue builtinRandomInt(const std::vector<MantraValue>& args);
MantraValue builtinChoice(const std::vector<MantraValue>& args);
}
