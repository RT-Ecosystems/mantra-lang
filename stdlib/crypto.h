#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinAesEncrypt(const std::vector<MantraValue>&);
MantraValue builtinAesDecrypt(const std::vector<MantraValue>&);
MantraValue builtinSha256(const std::vector<MantraValue>&);
MantraValue builtinMd5(const std::vector<MantraValue>&);
}
