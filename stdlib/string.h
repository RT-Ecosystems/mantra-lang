#pragma once

#include "../core/interpreter.h"

namespace mantra::stdlib {

MantraValue builtinUpper(const std::vector<MantraValue>& args);
MantraValue builtinLower(const std::vector<MantraValue>& args);
MantraValue builtinTrim(const std::vector<MantraValue>& args);
MantraValue builtinSplit(const std::vector<MantraValue>& args);
MantraValue builtinReplace(const std::vector<MantraValue>& args);
MantraValue builtinSlice(const std::vector<MantraValue>& args);
MantraValue builtinContains(const std::vector<MantraValue>& args);
MantraValue builtinStartsWith(const std::vector<MantraValue>& args);
MantraValue builtinEndsWith(const std::vector<MantraValue>& args);
MantraValue builtinRepeat(const std::vector<MantraValue>& args);

} // namespace mantra::stdlib
