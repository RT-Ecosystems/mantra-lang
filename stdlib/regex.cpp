#include "regex.h"
#include <regex>
namespace mantra::stdlib {
MantraValue builtinRegexMatch(const std::vector<MantraValue>& args) {
    if (args.size() < 2) return MantraValue::boolean(false);
    try { return MantraValue::boolean(std::regex_search(args[1].string_value, std::regex(args[0].string_value))); }
    catch (...) { return MantraValue::boolean(false); }
}
MantraValue builtinRegexReplace(const std::vector<MantraValue>& args) {
    if (args.size() < 3) return MantraValue::string("");
    try { return MantraValue::string(std::regex_replace(args[1].string_value, std::regex(args[0].string_value), args[2].string_value)); }
    catch (...) { return args[1]; }
}
}
