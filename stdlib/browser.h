#pragma once

#include "../core/interpreter.h"

namespace mantra::stdlib {

MantraValue builtinBrowserOpen(const std::vector<MantraValue>&);
MantraValue builtinBrowserClick(const std::vector<MantraValue>&);
MantraValue builtinBrowserType(const std::vector<MantraValue>&);
MantraValue builtinBrowserGetText(const std::vector<MantraValue>&);
MantraValue builtinBrowserScreenshot(const std::vector<MantraValue>&);
MantraValue builtinBrowserClose(const std::vector<MantraValue>&);

} // namespace mantra::stdlib
