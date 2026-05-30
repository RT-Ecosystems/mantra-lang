#include "csv.h"
#include <fstream>
#include <sstream>
namespace mantra::stdlib {
MantraValue builtinCsvRead(const std::vector<MantraValue>& args) {
    if (args.empty()) return MantraValue::string("");
    std::ifstream in(args[0].string_value);
    if (!in) return MantraValue::string("");
    std::ostringstream ss; ss << in.rdbuf();
    return MantraValue::string(ss.str());
}
MantraValue builtinCsvWrite(const std::vector<MantraValue>& args) {
    if (args.size() < 2) return MantraValue::boolean(false);
    std::ofstream out(args[0].string_value);
    if (!out) return MantraValue::boolean(false);
    out << args[1].string_value;
    return MantraValue::boolean(true);
}
}
