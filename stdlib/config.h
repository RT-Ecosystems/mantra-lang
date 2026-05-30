#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinConfigLoad(const std::vector<MantraValue>&);
MantraValue builtinConfigGet(const std::vector<MantraValue>&);
MantraValue builtinConfigSet(const std::vector<MantraValue>&);
MantraValue builtinConfigSave(const std::vector<MantraValue>&);
}
