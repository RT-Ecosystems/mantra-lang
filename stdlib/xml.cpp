#include "xml.h"
#include <string>
namespace mantra::stdlib {
MantraValue builtinXmlParse(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("");
    return MantraValue::string("[XML parsed: " + a[0].string_value.substr(0, 60) + "...]");
}
MantraValue builtinXmlGetNode(const std::vector<MantraValue>& a){
    if(a.size() < 2) return MantraValue::string("");
    return MantraValue::string("[Node: " + a[1].string_value + "]");
}
MantraValue builtinXmlSetNode(const std::vector<MantraValue>& a){
    if(a.size() < 3) return MantraValue::boolean(false);
    return MantraValue::boolean(true);
}
}
