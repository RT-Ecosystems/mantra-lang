#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinPdfCreate(const std::vector<MantraValue>&);
MantraValue builtinPdfText(const std::vector<MantraValue>&);
MantraValue builtinPdfImage(const std::vector<MantraValue>&);
MantraValue builtinPdfSave(const std::vector<MantraValue>&);
}
