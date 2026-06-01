#pragma once

#include "../core/interpreter.h"

namespace mantra::stdlib {

MantraValue builtinAppWindow(const std::vector<MantraValue>&);
MantraValue builtinAppButton(const std::vector<MantraValue>&);
MantraValue builtinAppLabel(const std::vector<MantraValue>&);
MantraValue builtinAppInput(const std::vector<MantraValue>&);
MantraValue builtinAppRun(const std::vector<MantraValue>&);

} // namespace mantra::stdlib
