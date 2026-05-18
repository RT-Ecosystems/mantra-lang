#include "mtr_parser.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace mantra::web {
namespace {

constexpr const char* kStructureSection = "dhanche";
constexpr const char* kStyleSection = "roop";
constexpr const char* kLogicSection = "kaam";

std::string trim(const std::string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

std::string lower(const std::string& text) {
    std::string out = text;
    for (char& ch : out) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return out;
}

} // namespace

MtrDocument MtrParser::parse(const std::string& source) const {
    MtrDocument doc;
    std::istringstream input(source);
    std::string line;
    std::string section;

    while (std::getline(input, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) {
            continue;
        }
        if (trimmed.front() == '[' && trimmed.back() == ']') {
            section = lower(trimmed.substr(1, trimmed.size() - 2));
            continue;
        }
        if (section == kStructureSection) {
            doc.structure += line + "\n";
        } else if (section == kStyleSection) {
            doc.style += line + "\n";
        } else if (section == kLogicSection) {
            doc.logic += line + "\n";
        }
    }

    return doc;
}

} // namespace mantra::web
