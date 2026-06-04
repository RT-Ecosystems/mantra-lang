#pragma once
#include "../include/mantra/core/types.h"
#include <string>
#include <vector>
#include <memory>

namespace mantra {
namespace core {

struct Module {
    std::string name;
    std::string path;
    std::string source;
    std::vector<std::string> exports;
    bool is_native = false;
};

class ModuleLoader;
ModuleLoader* getModuleLoader();

MantraValue builtinImport(const std::vector<MantraValue>& args);
MantraValue builtinExport(const std::vector<MantraValue>& args);
MantraValue builtinModuleExists(const std::vector<MantraValue>& args);
MantraValue builtinListModules(const std::vector<MantraValue>& args);

} // namespace core
} // namespace mantra