#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinImgLoad(const std::vector<MantraValue>&);
MantraValue builtinImgResize(const std::vector<MantraValue>&);
MantraValue builtinImgSave(const std::vector<MantraValue>&);
MantraValue builtinImgInfo(const std::vector<MantraValue>&);
}
