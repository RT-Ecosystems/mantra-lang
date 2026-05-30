#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinReadFile(const std::vector<MantraValue>& args);
MantraValue builtinWriteFile(const std::vector<MantraValue>& args);
MantraValue builtinFileExists(const std::vector<MantraValue>& args);
}
