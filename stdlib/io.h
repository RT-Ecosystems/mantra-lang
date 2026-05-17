#ifndef MANTRA_STDLIB_IO_H
#define MANTRA_STDLIB_IO_H

#include "../core/interpreter.h"
#include <vector>

namespace mantra::stdlib {

MantraValue builtinPrint(const std::vector<MantraValue>& args);
MantraValue builtinInput(const std::vector<MantraValue>& args);
MantraValue builtinReadFile(const std::vector<MantraValue>& args);
MantraValue builtinWriteFile(const std::vector<MantraValue>& args);

} // namespace mantra::stdlib

#endif // MANTRA_STDLIB_IO_H
