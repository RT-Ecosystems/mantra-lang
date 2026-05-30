#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinGetEnv(const std::vector<MantraValue>&);
MantraValue builtinSystem(const std::vector<MantraValue>&);
}
