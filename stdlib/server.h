#pragma once
#include "../core/interpreter.h"

namespace mantra::stdlib {

MantraValue builtinHttpServer(const std::vector<MantraValue>& args);
MantraValue builtinHttpRoute(const std::vector<MantraValue>& args);
MantraValue builtinHttpListen(const std::vector<MantraValue>& args);
MantraValue builtinHttpResponse(const std::vector<MantraValue>& args);

} // namespace mantra::stdlib
