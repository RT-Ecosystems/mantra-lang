#include "yaml.h"
#include <string>
namespace mantra::stdlib {
MantraValue builtinYamlLoad(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("");
    return MantraValue::string("[YAML loaded: " + a[0].string_value.substr(0, 60) + "...]");
}
MantraValue builtinYamlDump(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("{}");
    return MantraValue::string("[YAML dump of: " + a[0].string_value + "]");
}
}
