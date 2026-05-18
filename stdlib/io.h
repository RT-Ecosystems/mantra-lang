#ifndef MANTRA_STDLIB_IO_H
#define MANTRA_STDLIB_IO_H

#include "../core/interpreter.h"
#include <vector>

namespace mantra::stdlib {

MantraValue builtinPrint(const std::vector<MantraValue>& args);
MantraValue builtinInput(const std::vector<MantraValue>& args);
MantraValue builtinLength(const std::vector<MantraValue>& args);
MantraValue builtinConcat(const std::vector<MantraValue>& args);
MantraValue builtinToNumber(const std::vector<MantraValue>& args);
MantraValue builtinToString(const std::vector<MantraValue>& args);
MantraValue builtinType(const std::vector<MantraValue>& args);
MantraValue builtinExit(const std::vector<MantraValue>& args);
MantraValue builtinClear(const std::vector<MantraValue>& args);

} // namespace mantra::stdlib

#endif // MANTRA_STDLIB_IO_H
