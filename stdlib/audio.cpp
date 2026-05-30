#include "audio.h"
#include <cstdio>
#include <cstdlib>
#include <string>
namespace mantra::stdlib {
MantraValue builtinAudioLoad(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("");
    std::string cmd = "ffprobe -v quiet -print_format json -show_format "" + a[0].string_value + "" 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) return MantraValue::string("");
    char buf[512]; std::string result;
    while(fgets(buf, sizeof(buf), pipe)) result += buf;
    pclose(pipe);
    return MantraValue::string(result);
}
MantraValue builtinAudioPlay(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::boolean(false);
    std::string cmd = "ffplay -nodisp -autoexit "" + a[0].string_value + "" 2>/dev/null";
    return MantraValue::boolean(std::system(cmd.c_str()) == 0);
}
MantraValue builtinAudioInfo(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("");
    std::string cmd = "ffprobe -v quiet -print_format json -show_streams "" + a[0].string_value + "" 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) return MantraValue::string("");
    char buf[512]; std::string result;
    while(fgets(buf, sizeof(buf), pipe)) result += buf;
    pclose(pipe);
    return MantraValue::string(result);
}
}
