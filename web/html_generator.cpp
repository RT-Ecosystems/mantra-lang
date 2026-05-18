#include "html_generator.h"

#include <sstream>

namespace mantra::web {
namespace {

std::string escapeHtml(const std::string& text) {
    std::ostringstream out;
    for (char ch : text) {
        switch (ch) {
            case '&': out << "&amp;"; break;
            case '<': out << "&lt;"; break;
            case '>': out << "&gt;"; break;
            case '"': out << "&quot;"; break;
            default: out << ch; break;
        }
    }
    return out.str();
}

std::string tagForKey(const std::string& key) {
    if (key == "sheershak") return "h1";
    if (key == "para") return "p";
    if (key == "button") return "button";
    return "div";
}

std::string renderStructure(const std::string& structure) {
    std::istringstream input(structure);
    std::string line;
    std::ostringstream out;
    while (std::getline(input, line)) {
        auto eq = line.find('=');
        if (eq == std::string::npos) {
            continue;
        }
        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);
        auto trim = [](std::string s) {
            auto start = s.find_first_not_of(" \t");
            if (start == std::string::npos) return std::string{};
            auto end = s.find_last_not_of(" \t");
            return s.substr(start, end - start + 1);
        };
        key = trim(key);
        value = trim(value);
        if (!value.empty() && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }
        std::string tag = tagForKey(key);
        if (tag == "button" && value.empty()) {
            value = "Button";
        }
        out << "<" << tag;
        if (tag == "button") {
            out << " type=\"button\"";
        }
        out << ">" << escapeHtml(value) << "</" << tag << ">\n";
    }
    return out.str();
}

} // namespace

std::string HtmlGenerator::generate(const MtrDocument& document) const {
    std::ostringstream html;
    html << "<!doctype html>\n<html lang=\"en\">\n<head>\n";
    html << "<meta charset=\"UTF-8\">\n";
    html << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "<title>MANTRA Web</title>\n";
    html << "<style>\n" << document.style << "\n</style>\n";
    html << "</head>\n<body>\n";
    html << "<div class=\"mtr-root\">\n" << renderStructure(document.structure) << "</div>\n";
    html << "<script>\n" << document.logic << "\n</script>\n";
    html << "</body>\n</html>\n";
    return html.str();
}

} // namespace mantra::web
