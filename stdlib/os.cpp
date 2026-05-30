#include "os.h"
#include <cstdlib>
namespace mantra::stdlib {
MantraValue builtinGetEnv(const std::vector<MantraValue>& args) {
    if (args.empty()) return MantraValue::string("");
    const char* val = std::getenv(args[0].string_value.c_str());
    return MantraValue::string(val ? val : "");
}
MantraValue builtinSystem(const std::vector<MantraValue>& args) {
    if (args.empty()) return MantraValue::number(-1);
    return MantraValue::number(std::system(args[0].string_value.c_str()));
}
}
