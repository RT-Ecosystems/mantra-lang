#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinGetEnv(const std::vector<MantraValue>& args);
MantraValue builtinSystem(const std::vector<MantraValue>& args);
}
