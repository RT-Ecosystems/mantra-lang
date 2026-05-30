#include "config.h"
#include <fstream>
#include <sstream>
#include <map>
namespace mantra::stdlib {
static std::map<std::string, std::string> g_config;
MantraValue builtinConfigLoad(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::boolean(false);
    std::ifstream in(a[0].string_value);
    if(!in) return MantraValue::boolean(false);
    g_config.clear();
    std::string line;
    while(std::getline(in, line)){
        size_t eq = line.find('=');
        if(eq != std::string::npos)
            g_config[line.substr(0,eq)] = line.substr(eq+1);
    }
    return MantraValue::boolean(true);
}
MantraValue builtinConfigGet(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("");
    return MantraValue::string(g_config[a[0].string_value]);
}
MantraValue builtinConfigSet(const std::vector<MantraValue>& a){
    if(a.size() < 2) return MantraValue::boolean(false);
    g_config[a[0].string_value] = a[1].string_value;
    return MantraValue::boolean(true);
}
MantraValue builtinConfigSave(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::boolean(false);
    std::ofstream out(a[0].string_value);
    if(!out) return MantraValue::boolean(false);
    for(auto& p : g_config) out << p.first << "=" << p.second << "\n";
    return MantraValue::boolean(true);
}
}
