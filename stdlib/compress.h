#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinGzip(const std::vector<MantraValue>&);
MantraValue builtinGunzip(const std::vector<MantraValue>&);
MantraValue builtinBzip2(const std::vector<MantraValue>&);
MantraValue builtinBunzip2(const std::vector<MantraValue>&);
}
