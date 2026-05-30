#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinEmailSend(const std::vector<MantraValue>&);
MantraValue builtinEmailSetServer(const std::vector<MantraValue>&);
}
