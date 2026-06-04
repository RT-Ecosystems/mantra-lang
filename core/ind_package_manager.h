#pragma once
#include "../include/mantra/core/types.h"
#include <string>
#include <vector>

namespace mantra {
namespace core {

class INDPackageManager;
INDPackageManager* getINDManager();

MantraValue builtinIndInstall(const std::vector<MantraValue>& args);
MantraValue builtinIndRemove(const std::vector<MantraValue>& args);
MantraValue builtinIndList(const std::vector<MantraValue>& args);
MantraValue builtinIndSearch(const std::vector<MantraValue>& args);
MantraValue builtinIndUpdate(const std::vector<MantraValue>& args);
MantraValue builtinIndInit(const std::vector<MantraValue>& args);

} // namespace core
} // namespace mantra