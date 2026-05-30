#include "json.h"
#include <sstream>
namespace mantra::stdlib {
MantraValue builtinJsonParse(const std::vector<MantraValue>& args) {
    if (args.empty()) return MantraValue::string("");
    return MantraValue::string("[parsed] " + args[0].string_value);
}
MantraValue builtinJsonStringify(const std::vector<MantraValue>& args) {
    if (args.empty()) return MantraValue::string("{}");
    return MantraValue::string("{\"value\": \"" + args[0].string_value + "\"}");
}
}
