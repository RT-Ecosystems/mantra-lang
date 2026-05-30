#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinSha1(const std::vector<MantraValue>&);
MantraValue builtinSha512(const std::vector<MantraValue>&);
MantraValue builtinCrc32(const std::vector<MantraValue>&);
}
