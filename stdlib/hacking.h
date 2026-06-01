#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinPortScan(const std::vector<MantraValue>&);
MantraValue builtinNetworkSniff(const std::vector<MantraValue>&);
MantraValue builtinPingHost(const std::vector<MantraValue>&);
MantraValue builtinWhoisLookup(const std::vector<MantraValue>&);
MantraValue builtinDnsLookup(const std::vector<MantraValue>&);
MantraValue builtinHashCrack(const std::vector<MantraValue>&);
}
