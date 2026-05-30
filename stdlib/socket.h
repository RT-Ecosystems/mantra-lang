#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinTcpConnect(const std::vector<MantraValue>&);
MantraValue builtinTcpSend(const std::vector<MantraValue>&);
MantraValue builtinTcpRecv(const std::vector<MantraValue>&);
MantraValue builtinTcpClose(const std::vector<MantraValue>&);
MantraValue builtinTcpListen(const std::vector<MantraValue>&);
}
