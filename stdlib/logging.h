#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinLogInfo(const std::vector<MantraValue>&);
MantraValue builtinLogError(const std::vector<MantraValue>&);
MantraValue builtinLogWarn(const std::vector<MantraValue>&);
MantraValue builtinLogSetFile(const std::vector<MantraValue>&);
}
