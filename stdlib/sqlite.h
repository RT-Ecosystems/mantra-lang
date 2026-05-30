#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinSqlOpen(const std::vector<MantraValue>&);
MantraValue builtinSqlExec(const std::vector<MantraValue>&);
MantraValue builtinSqlClose(const std::vector<MantraValue>&);
}
