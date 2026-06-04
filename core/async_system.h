#pragma once
#include "../include/mantra/core/types.h"
#include <string>

namespace mantra {
namespace core {

class EventLoop;
EventLoop* getEventLoop();

MantraValue builtinAsync(const std::vector<MantraValue>& args);
MantraValue builtinAwait(const std::vector<MantraValue>& args);
MantraValue builtinSleep(const std::vector<MantraValue>& args);
MantraValue builtinIsComplete(const std::vector<MantraValue>& args);

} // namespace core
} // namespace mantra