#include "ind_package_manager.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

namespace mantra {
namespace core {

struct INDConfig {
    std::string registry_url = "https://api.github.com/repos/RT-Ecosystems/mantra-packages/contents/packages";
    std::string install_dir = "./modules";
    std::string cache_dir = "./.ind/cache";
};

static INDConfig g_ind_config;

struct PackageInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string url;
    std::vector<std::string> dependencies;
    std::string checksum;
};

class INDPackageManager {
public:
    INDPackageManager() {
        std::filesystem::create_directories(g_ind_config.install_dir);
        std::filesystem::create_directories(g_ind_config.cache_dir);
    }

    bool install(const std::string& package_name, const std::string& version = "latest") {
        std::cout << "Installing " << package_name << "..." << std::endl;
        if (isInstalled(package_name)) {
            std::cout << "Already installed: " << package_name << std::endl;
            return true;
        }
        std::string repo_url = "https://github.com/RT-Ecosystems/mantra-" + package_name;
        if (downloadFromGitHub(repo_url, package_name)) {
            std::cout << "Installed: " << package_name << std::endl;
            return true;
        }
        std::cerr << "Failed to install: " << package_name << std::endl;
        return false;
    }

    bool remove(const std::string& package_name) {
        std::string package_dir = g_ind_config.install_dir + "/" + package_name;
        if (std::filesystem::exists(package_dir)) {
            std::filesystem::remove_all(package_dir);
            std::cout << "Removed: " << package_name << std::endl;
            return true;
        }
        return false;
    }

    std::vector<std::string> list() const {
        std::vector<std::string> packages;
        if (std::filesystem::exists(g_ind_config.install_dir)) {
            for (const auto& entry : std::filesystem::directory_iterator(g_ind_config.install_dir)) {
                if (entry.is_directory()) packages.push_back(entry.path().filename().string());
            }
        }
        return packages;
    }

    std::vector<PackageInfo> search(const std::string& query) {
        std::vector<PackageInfo> results;
        PackageInfo info;
        info.name = "networking";
        info.version = "1.0.0";
        info.description = "HTTP client and server utilities";
        info.author = "RT-Ecosystems";
        results.push_back(info);
        return results;
    }

    bool update(const std::string& package_name) {
        if (remove(package_name)) return install(package_name);
        return false;
    }

    bool updateAll() {
        auto packages = list();
        bool success = true;
        for (const auto& pkg : packages) if (!update(pkg)) success = false;
        return success;
    }

    bool isInstalled(const std::string& package_name) const {
        return std::filesystem::exists(g_ind_config.install_dir + "/" + package_name);
    }

    PackageInfo getInfo(const std::string& package_name) {
        PackageInfo info;
        info.name = package_name;
        std::string manifest_path = g_ind_config.install_dir + "/" + package_name + "/manifest.json";
        if (std::filesystem::exists(manifest_path)) {
            std::ifstream file(manifest_path);
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            info.version = "1.0.0";
            info.description = "A MANTRA package";
            info.author = "RT-Ecosystems";
        }
        return info;
    }

    bool init(const std::string& package_name, const std::string& author = "") {
        std::string package_dir = g_ind_config.install_dir + "/" + package_name;
        std::filesystem::create_directories(package_dir);

        std::ofstream manifest_file(package_dir + "/manifest.json");
        manifest_file << "{\n";
        manifest_file << "  \"name\": \"" << package_name << "\",\n";
        manifest_file << "  \"version\": \"1.0.0\",\n";
        manifest_file << "  \"description\": \"A MANTRA package\",\n";
        manifest_file << "  \"author\": \"" << (author.empty() ? "Anonymous" : author) << "\",\n";
        manifest_file << "  \"main\": \"index.mtr\",\n";
        manifest_file << "  \"dependencies\": []\n";
        manifest_file << "}\n";

        std::ofstream index_file(package_dir + "/index.mtr");
        index_file << "// " << package_name << " package\n";
        index_file << "export kaam hello() {\n";
        index_file << "    dikhao \"Hello from " << package_name << "!\"\n";
        index_file << "}\n";

        std::cout << "Initialized package: " << package_name << std::endl;
        return true;
    }

private:
    bool downloadFromGitHub(const std::string& repo_url, const std::string& package_name) {
        std::string clone_cmd = "git clone " + repo_url + " " + g_ind_config.install_dir + "/" + package_name + " 2>/dev/null";
        int result = system(clone_cmd.c_str());
        return (result == 0);
    }
};

static std::unique_ptr<INDPackageManager> g_ind_manager;

INDPackageManager* getINDManager() {
    if (!g_ind_manager) g_ind_manager = std::make_unique<INDPackageManager>();
    return g_ind_manager.get();
}

MantraValue builtinIndInstall(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING) throw std::runtime_error("ind_install: package name required");
    std::string version = "latest";
    if (args.size() >= 2 && args[1].type == MantraValue::Type::STRING) version = args[1].string_value;
    return MantraValue::boolean(getINDManager()->install(args[0].string_value, version));
}

MantraValue builtinIndRemove(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING) throw std::runtime_error("ind_remove: package name required");
    return MantraValue::boolean(getINDManager()->remove(args[0].string_value));
}

MantraValue builtinIndList(const std::vector<MantraValue>& args) {
    auto packages = getINDManager()->list();
    MantraValue result = MantraValue::array();
    for (const auto& pkg : packages) result.array_value.push_back(MantraValue::string(pkg));
    return result;
}

MantraValue builtinIndSearch(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING) return MantraValue::array();
    auto results = getINDManager()->search(args[0].string_value);
    MantraValue result = MantraValue::array();
    for (const auto& info : results) {
        MantraValue pkg = MantraValue::object();
        pkg.object_value["name"] = MantraValue::string(info.name);
        pkg.object_value["version"] = MantraValue::string(info.version);
        pkg.object_value["description"] = MantraValue::string(info.description);
        result.array_value.push_back(pkg);
    }
    return result;
}

MantraValue builtinIndUpdate(const std::vector<MantraValue>& args) {
    if (args.empty()) return MantraValue::boolean(getINDManager()->updateAll());
    if (args[0].type != MantraValue::Type::STRING) throw std::runtime_error("ind_update: package name required");
    return MantraValue::boolean(getINDManager()->update(args[0].string_value));
}

MantraValue builtinIndInit(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING) throw std::runtime_error("ind_init: package name required");
    std::string author = "";
    if (args.size() >= 2 && args[1].type == MantraValue::Type::STRING) author = args[1].string_value;
    return MantraValue::boolean(getINDManager()->init(args[0].string_value, author));
}

} // namespace core
} // namespace mantra