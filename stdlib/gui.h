#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinGuiWindow(const std::vector<MantraValue>&);
MantraValue builtinGuiButton(const std::vector<MantraValue>&);
MantraValue builtinGuiLabel(const std::vector<MantraValue>&);
MantraValue builtinGuiRun(const std::vector<MantraValue>&);
}
