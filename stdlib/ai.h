#pragma once

#include "../core/interpreter.h"

namespace mantra::stdlib {

// AI / NeuroBrain built-in functions
MantraValue builtinBeejCreate(const std::vector<MantraValue>& args);
MantraValue builtinGyanCreate(const std::vector<MantraValue>& args);
MantraValue builtinVidyarthiCreate(const std::vector<MantraValue>& args);
MantraValue builtinSikho(const std::vector<MantraValue>& args);
MantraValue builtinPuchho(const std::vector<MantraValue>& args);
MantraValue builtinHash(const std::vector<MantraValue>& args);
MantraValue builtinSimilar(const std::vector<MantraValue>& args);
MantraValue builtinSaveModel(const std::vector<MantraValue>& args);
MantraValue builtinLoadModel(const std::vector<MantraValue>& args);

} // namespace mantra::stdlib
