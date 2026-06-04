#include "module_system.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>

namespace mantra {
namespace core {

static std::unordered_map<std::string, std::shared_ptr<Module>> module_cache;

class ModuleLoader {
public:
    std::string base_path;
    ModuleLoader(const std::string& path = "./modules") : base_path(path) {}

    std::shared_ptr<Module> load(const std::string& name) {
        auto it = module_cache.find(name);
        if (it != module_cache.end()) return it->second;

        std::vector<std::string> search_paths = {
            base_path + "/" + name + ".mtr",
            base_path + "/" + name + "/index.mtr",
            base_path + "/" + name + "/" + name + ".mtr",
            "./stdlib/" + name + ".cpp",
            "./modules/" + name + ".mtr",
            "./lib/" + name + ".mtr",
        };

        for (const auto& path : search_paths) {
            if (std::filesystem::exists(path)) {
                auto module = parseModule(path, name);
                module_cache[name] = module;
                return module;
            }
        }
        throw std::runtime_error("Module not found: " + name);
    }

    bool exists(const std::string& name) const {
        std::vector<std::string> search_paths = {
            base_path + "/" + name + ".mtr",
            "./stdlib/" + name + ".cpp",
            "./modules/" + name + ".mtr",
        };
        for (const auto& path : search_paths) {
            if (std::filesystem::exists(path)) return true;
        }
        return false;
    }

    std::vector<std::string> listModules() const {
        std::vector<std::string> modules;
        if (std::filesystem::exists(base_path)) {
            for (const auto& entry : std::filesystem::directory_iterator(base_path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".mtr") {
                    modules.push_back(entry.path().stem().string());
                }
            }
        }
        return modules;
    }

private:
    std::shared_ptr<Module> parseModule(const std::string& path, const std::string& name) {
        auto module = std::make_shared<Module>();
        module->name = name;
        module->path = path;
        std::ifstream file(path);
        if (!file.is_open()) throw std::runtime_error("Cannot open module: " + path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        module->source = buffer.str();
        parseExports(module);
        return module;
    }

    void parseExports(std::shared_ptr<Module> module) {
        std::istringstream iss(module->source);
        std::string line;
        while (std::getline(iss, line)) {
            size_t export_pos = line.find("export");
            if (export_pos != std::string::npos) {
                size_t kaam_pos = line.find("kaam", export_pos);
                size_t function_pos = line.find("function", export_pos);
                if (kaam_pos != std::string::npos || function_pos != std::string::npos) {
                    size_t name_start = line.find_first_not_of(" \t", std::max(kaam_pos, function_pos) + 4);
                    if (name_start != std::string::npos) {
                        size_t name_end = line.find_first_of(" \t(", name_start);
                        std::string func_name = line.substr(name_start, name_end - name_start);
                        module->exports.push_back(func_name);
                    }
                }
            }
        }
    }
};

static std::unique_ptr<ModuleLoader> g_module_loader;

ModuleLoader* getModuleLoader() {
    if (!g_module_loader) g_module_loader = std::make_unique<ModuleLoader>();
    return g_module_loader.get();
}

MantraValue builtinImport(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING) {
        throw std::runtime_error("import: module name string required");
    }
    std::string module_name = args[0].string_value;
    try {
        auto module = getModuleLoader()->load(module_name);
        MantraValue module_obj = MantraValue::object();
        module_obj.object_value["__name__"] = MantraValue::string(module->name);
        module_obj.object_value["__path__"] = MantraValue::string(module->path);
        for (const auto& export_name : module->exports) {
            module_obj.object_value[export_name] = MantraValue::string("<function " + export_name + ">");
        }
        return module_obj;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Import error: ") + e.what());
    }
}

MantraValue builtinExport(const std::vector<MantraValue>& args) {
    MantraValue result = MantraValue::array();
    for (const auto& arg : args) {
        if (arg.type == MantraValue::Type::STRING) result.array_value.push_back(arg);
    }
    return result;
}

MantraValue builtinModuleExists(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING) return MantraValue::boolean(false);
    return MantraValue::boolean(getModuleLoader()->exists(args[0].string_value));
}

MantraValue builtinListModules(const std::vector<MantraValue>& args) {
    auto modules = getModuleLoader()->listModules();
    MantraValue result = MantraValue::array();
    for (const auto& mod : modules) result.array_value.push_back(MantraValue::string(mod));
    return result;
}

} // namespace core
} // namespace mantra