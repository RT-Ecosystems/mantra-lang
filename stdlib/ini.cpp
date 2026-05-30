#include "ini.h"
#include <fstream>
#include <sstream>
#include <string>
namespace mantra::stdlib {
MantraValue builtinIniRead(const std::vector<MantraValue>& a){
    if(a.size() < 2) return MantraValue::string("");
    std::ifstream in(a[0].string_value);
    if(!in) return MantraValue::string("");
    std::string line, key = a[1].string_value;
    while(std::getline(in, line)){
        size_t eq = line.find('=');
        if(eq != std::string::npos && line.substr(0, eq) == key){
            return MantraValue::string(line.substr(eq+1));
        }
    }
    return MantraValue::string("");
}
MantraValue builtinIniWrite(const std::vector<MantraValue>& a){
    if(a.size() < 3) return MantraValue::boolean(false);
    std::ofstream out(a[0].string_value);
    if(!out) return MantraValue::boolean(false);
    out << a[1].string_value << "=" << a[2].string_value << "\n";
    return MantraValue::boolean(true);
}
}
