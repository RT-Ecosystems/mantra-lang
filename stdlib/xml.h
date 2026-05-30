#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinXmlParse(const std::vector<MantraValue>&);
MantraValue builtinXmlGetNode(const std::vector<MantraValue>&);
MantraValue builtinXmlSetNode(const std::vector<MantraValue>&);
}
