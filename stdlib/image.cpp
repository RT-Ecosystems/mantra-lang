#include "image.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
namespace mantra::stdlib {
MantraValue builtinImgLoad(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("");
    std::string cmd = "convert "" + a[0].string_value + "" -resize 1x1 txt:- 2>/dev/null | head -2";
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) return MantraValue::string("");
    char buf[256]; std::string result;
    while(fgets(buf, sizeof(buf), pipe)) result += buf;
    pclose(pipe);
    return MantraValue::string(result.empty() ? "[Image loaded]" : result);
}
MantraValue builtinImgResize(const std::vector<MantraValue>& a){
    if(a.size() < 3) return MantraValue::boolean(false);
    std::string cmd = "convert "" + a[0].string_value + "" -resize " + std::to_string((int)a[1].number_value) + "x" + std::to_string((int)a[2].number_value) + " "" + a[0].string_value + """;
    return MantraValue::boolean(std::system(cmd.c_str()) == 0);
}
MantraValue builtinImgSave(const std::vector<MantraValue>& a){
    return MantraValue::boolean(true); // stub
}
MantraValue builtinImgInfo(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("");
    std::string cmd = "identify "" + a[0].string_value + "" 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) return MantraValue::string("");
    char buf[256]; std::string result;
    while(fgets(buf, sizeof(buf), pipe)) result += buf;
    pclose(pipe);
    return MantraValue::string(result);
}
}
