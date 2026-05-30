#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinChartBar(const std::vector<MantraValue>&);
MantraValue builtinChartLine(const std::vector<MantraValue>&);
MantraValue builtinChartPie(const std::vector<MantraValue>&);
MantraValue builtinChartSave(const std::vector<MantraValue>&);
}
