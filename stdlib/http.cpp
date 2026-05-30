#include "http.h"
#include <cstdio>
#include <string>
namespace mantra::stdlib {
MantraValue builtinHttpGet(const std::vector<MantraValue>& args) {
    if (args.empty()) return MantraValue::string("");
    std::string cmd = "curl -s \"" + args[0].string_value + "\" 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return MantraValue::string("");
    char buf[128];
    std::string result;
    while (fgets(buf, sizeof(buf), pipe)) result += buf;
    pclose(pipe);
    return MantraValue::string(result);
}
MantraValue builtinHttpPost(const std::vector<MantraValue>& args) {
    return MantraValue::string("[POST not implemented]");
}
}
