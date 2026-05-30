#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinNow(const std::vector<MantraValue>& args);
MantraValue builtinToday(const std::vector<MantraValue>& args);
}
