#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinZipCreate(const std::vector<MantraValue>&);
MantraValue builtinZipExtract(const std::vector<MantraValue>&);
MantraValue builtinZipList(const std::vector<MantraValue>&);
}
