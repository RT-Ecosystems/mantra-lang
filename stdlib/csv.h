#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinCsvRead(const std::vector<MantraValue>& args);
MantraValue builtinCsvWrite(const std::vector<MantraValue>& args);
}
