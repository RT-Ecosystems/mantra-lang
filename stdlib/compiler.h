#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinLexerTokenize(const std::vector<MantraValue>&);
MantraValue builtinParserParse(const std::vector<MantraValue>&);
MantraValue builtinCodeGen(const std::vector<MantraValue>&);
MantraValue builtinOptimizeIR(const std::vector<MantraValue>&);
MantraValue builtinEmitBinary(const std::vector<MantraValue>&);
}
