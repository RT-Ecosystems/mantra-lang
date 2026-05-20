#include "html_generator.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace mantra {

HtmlGenerator::HtmlGenerator() {}

std::string HtmlGenerator::EscapeHtml(const std::string& s) const {
    std::string out;
    for (char c : s) {
        switch(c) {
            case '<': out += "&lt;"; break;
            case '>': out += "&gt;"; break;
            case '&': out += "&amp;"; break;
            case '"': out += "&quot;"; break;
            default: out += c;
        }
    }
    return out;
}

std::string HtmlGenerator::MakeSafeId(const std::string& s) const {
    std::string out;
    for (char c : s) {
        if (std::isalnum(c) || c == '-' || c == '_')
            out += std::tolower(c);
        else if (c == ' ')
            out += '_';
    }
    return out.empty() ? "elem" : out;
}

std::string HtmlGenerator::RenderElement(const MtrElement& el) const {
    std::string safe_id = MakeSafeId(el.id);
    std::string out;

    if (el.type == "img") {
        out = "<img id=\"" + safe_id + "\" src=\"" +
              EscapeHtml(el.content) + "\" alt=\"image\" "
              "class=\"mantra-img\">\n";
    } else if (el.type == "input") {
        out = "<input id=\"" + safe_id + "\" "
              "placeholder=\"" + EscapeHtml(el.content) + "\" "
              "class=\"mantra-input\">\n";
    } else if (el.type == "a") {
        out = "<a id=\"" + safe_id + "\" href=\"#\" "
              "class=\"mantra-link\">" +
              EscapeHtml(el.content) + "</a>\n";
    } else if (el.type == "ul") {
        out = "<ul id=\"" + safe_id + "\" class=\"mantra-list\">\n";
        for (const auto& child : el.children)
            out += "  " + RenderElement(child);
        out += "</ul>\n";
    } else {
        out = "<" + el.type + " id=\"" + safe_id + "\" "
              "class=\"mantra-" + el.type + "\">" +
              EscapeHtml(el.content) +
              "</" + el.type + ">\n";
    }
    return out;
}

std::string HtmlGenerator::GenerateHtml(const MtrDocument& doc) const {
    std::string out;
    for (const auto& el : doc.elements)
        out += "    " + RenderElement(el);
    return out;
}

std::string HtmlGenerator::GenerateCss(const MtrDocument& doc) const {
    std::ostringstream css;

    // Base responsive styles
    css << "    * { box-sizing: border-box; margin: 0; padding: 0; }\n";
    css << "    body {\n";
    css << "      font-family: 'Segoe UI', sans-serif;\n";
    css << "      background: #f8f9fa;\n";
    css << "      color: #333;\n";
    css << "      min-height: 100vh;\n";
    css << "    }\n";
    css << "    .mantra-container {\n";
    css << "      max-width: 1100px;\n";
    css << "      margin: 0 auto;\n";
    css << "      padding: 40px 20px;\n";
    css << "    }\n";
    css << "    .mantra-h1 {\n";
    css << "      font-size: 2.5rem;\n";
    css << "      font-weight: 700;\n";
    css << "      margin-bottom: 16px;\n";
    css << "      color: #FF6B00;\n";
    css << "    }\n";
    css << "    .mantra-p {\n";
    css << "      font-size: 1.1rem;\n";
    css << "      line-height: 1.7;\n";
    css << "      margin-bottom: 16px;\n";
    css << "    }\n";
    css << "    .mantra-button {\n";
    css << "      background: #FF6B00;\n";
    css << "      color: white;\n";
    css << "      border: none;\n";
    css << "      padding: 12px 28px;\n";
    css << "      border-radius: 6px;\n";
    css << "      font-size: 1rem;\n";
    css << "      cursor: pointer;\n";
    css << "      transition: 0.2s;\n";
    css << "      margin: 8px 0;\n";
    css << "    }\n";
    css << "    .mantra-button:hover { opacity: 0.85; }\n";
    css << "    .mantra-input {\n";
    css << "      padding: 10px 16px;\n";
    css << "      border: 2px solid #ddd;\n";
    css << "      border-radius: 6px;\n";
    css << "      font-size: 1rem;\n";
    css << "      width: 100%;\n";
    css << "      margin: 8px 0;\n";
    css << "    }\n";
    css << "    .mantra-link {\n";
    css << "      color: #FF6B00;\n";
    css << "      text-decoration: none;\n";
    css << "    }\n";
    css << "    .mantra-list {\n";
    css << "      padding-left: 24px;\n";
    css << "      margin: 12px 0;\n";
    css << "    }\n";
    css << "    .mantra-img {\n";
    css << "      max-width: 100%;\n";
    css << "      border-radius: 8px;\n";
    css << "    }\n";
    css << "    .mantra-div {\n";
    css << "      margin: 16px 0;\n";
    css << "      padding: 20px;\n";
    css << "      background: white;\n";
    css << "      border-radius: 10px;\n";
    css << "      box-shadow: 0 2px 8px rgba(0,0,0,0.08);\n";
    css << "    }\n";
    css << "    @media(max-width:600px) {\n";
    css << "      .mantra-h1 { font-size: 1.8rem; }\n";
    css << "      .mantra-container { padding: 20px 12px; }\n";
    css << "    }\n";

    // User-defined styles from [roop]
    for (const auto& [selector, props] : doc.styles) {
        // Find matching element IDs
        css << "    #" << selector;
        // Also try with _N suffix pattern
        css << ", [id^=\"" << selector << "_\"] {\n";
        for (const auto& [prop, val] : props) {
            std::string css_val = val;
            // Remove quotes if present
            if (css_val.size() >= 2 &&
                css_val.front() == '"' && css_val.back() == '"')
                css_val = css_val.substr(1, css_val.size()-2);
            css << "      " << prop << ": " << css_val << ";\n";
        }
        css << "    }\n";
    }

    return css.str();
}

std::string HtmlGenerator::GenerateJs(const MtrDocument& doc) const {
    std::ostringstream js;
    js << "    document.addEventListener('DOMContentLoaded', function() {\n";

    for (const auto& ev : doc.events) {
        std::string safe_id = MakeSafeId(ev.element_id);
        js << "      (function() {\n";
        js << "        var el = document.getElementById('"
           << safe_id << "');\n";
        js << "        if (!el) {\n";
        js << "          // Try prefix match\n";
        js << "          el = document.querySelector('[id^=\""
           << safe_id << "\"]');\n";
        js << "        }\n";
        js << "        if (el) {\n";
        js << "          el.addEventListener('" << ev.event_type
           << "', function() {\n";

        if (ev.action == "alert") {
            js << "            alert('"
               << ev.value << "');\n";
        } else if (ev.action == "navigate") {
            js << "            window.location.href = '"
               << ev.value << "';\n";
        } else if (ev.action == "console.log") {
            js << "            console.log('"
               << ev.value << "');\n";
        } else if (ev.action == "change") {
            js << "            el.textContent = '"
               << ev.value << "';\n";
        } else {
            js << "            alert('"
               << ev.value << "');\n";
        }

        js << "          });\n";
        js << "        }\n";
        js << "      })();\n";
    }

    js << "    });\n";
    return js.str();
}

std::string HtmlGenerator::Generate(const MtrDocument& doc,
                                     const std::string& title) {
    std::ostringstream html;

    html << "<!DOCTYPE html>\n";
    html << "<html lang=\"hi\">\n";
    html << "<head>\n";
    html << "  <meta charset=\"UTF-8\">\n";
    html << "  <meta name=\"viewport\" "
            "content=\"width=device-width, initial-scale=1.0\">\n";
    html << "  <meta name=\"generator\" content=\"MANTRA v0.1.0\">\n";
    html << "  <title>" << EscapeHtml(title) << "</title>\n";
    html << "  <style>\n";
    html << GenerateCss(doc);
    html << "  </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "  <div class=\"mantra-container\">\n";
    html << GenerateHtml(doc);
    html << "  </div>\n";
    html << "  <script>\n";
    html << GenerateJs(doc);
    html << "  </script>\n";
    html << "</body>\n";
    html << "</html>\n";

    return html.str();
}

bool HtmlGenerator::GenerateFile(const MtrDocument& doc,
                                  const std::string& output_path,
                                  const std::string& title) {
    std::string html = Generate(doc, title);
    std::ofstream out(output_path);
    if (!out.is_open()) return false;
    out << html;
    return true;
}

} // namespace mantra
