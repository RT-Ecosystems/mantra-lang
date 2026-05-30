#include "zip.h"
#include <cstdio>
#include <cstdlib>
#include <string>
namespace mantra::stdlib {
MantraValue builtinZipCreate(const std::vector<MantraValue>& a){
    if(a.size() < 2) return MantraValue::boolean(false);
    std::string cmd = "zip -r "" + a[0].string_value + "" "" + a[1].string_value + "" 2>/dev/null";
    return MantraValue::boolean(std::system(cmd.c_str()) == 0);
}
MantraValue builtinZipExtract(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::boolean(false);
    std::string dest = (a.size() > 1) ? a[1].string_value : ".";
    std::string cmd = "unzip -o "" + a[0].string_value + "" -d "" + dest + "" 2>/dev/null";
    return MantraValue::boolean(std::system(cmd.c_str()) == 0);
}
MantraValue builtinZipList(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("");
    std::string cmd = "unzip -l "" + a[0].string_value + "" 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) return MantraValue::string("");
    char buf[256]; std::string result;
    while(fgets(buf, sizeof(buf), pipe)) result += buf;
    pclose(pipe);
    return MantraValue::string(result);
}
}
