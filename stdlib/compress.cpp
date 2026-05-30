#include "compress.h"
#include <cstdio>
#include <cstdlib>
#include <string>
namespace mantra::stdlib {
MantraValue builtinGzip(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::boolean(false);
    std::string cmd = "gzip -c "" + a[0].string_value + "" 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) return MantraValue::string("");
    char buf[256]; std::string result;
    while(fgets(buf, sizeof(buf), pipe)) result += buf;
    pclose(pipe);
    return MantraValue::string(result);
}
MantraValue builtinGunzip(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::boolean(false);
    std::string cmd = "gunzip -c "" + a[0].string_value + "" 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) return MantraValue::string("");
    char buf[256]; std::string result;
    while(fgets(buf, sizeof(buf), pipe)) result += buf;
    pclose(pipe);
    return MantraValue::string(result);
}
MantraValue builtinBzip2(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::boolean(false);
    std::string cmd = "bzip2 -c "" + a[0].string_value + "" 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) return MantraValue::string("");
    char buf[256]; std::string result;
    while(fgets(buf, sizeof(buf), pipe)) result += buf;
    pclose(pipe);
    return MantraValue::string(result);
}
MantraValue builtinBunzip2(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::boolean(false);
    std::string cmd = "bunzip2 -c "" + a[0].string_value + "" 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) return MantraValue::string("");
    char buf[256]; std::string result;
    while(fgets(buf, sizeof(buf), pipe)) result += buf;
    pclose(pipe);
    return MantraValue::string(result);
}
}
