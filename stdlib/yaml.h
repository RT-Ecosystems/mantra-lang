#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinYamlLoad(const std::vector<MantraValue>&);
MantraValue builtinYamlDump(const std::vector<MantraValue>&);
}
