#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinProcessRun(const std::vector<MantraValue>&);
MantraValue builtinProcessOutput(const std::vector<MantraValue>&);
MantraValue builtinProcessWait(const std::vector<MantraValue>&);
MantraValue builtinProcessKill(const std::vector<MantraValue>&);
}
