#include "mtr_parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace mantra {

MtrParser::MtrParser() {}

void MtrParser::EnsureMapsInitialized() {
    if (mapsInitialized_) return;
    mapsInitialized_ = true;

    // ---- Section names for all 22 languages ----
    // Structure sections
    auto S = SectionType::Structure;
    auto St = SectionType::Style;
    auto L = SectionType::Logic;

    // Hinglish
    sectionMap_["dhanche"]   = {S, UserLanguage::Hinglish};
    sectionMap_["roop"]      = {St,UserLanguage::Hinglish};
    sectionMap_["kaam"]      = {L, UserLanguage::Hinglish};
    // Tanglish
    sectionMap_["amaippu"]   = {S, UserLanguage::Tanglish};
    sectionMap_["vadivu"]    = {St,UserLanguage::Tanglish};
    sectionMap_["seyal"]     = {L, UserLanguage::Tanglish};
    // Punglish
    sectionMap_["dhanche"]   = {S, UserLanguage::Punglish};
    sectionMap_["roop"]      = {St,UserLanguage::Punglish};
    sectionMap_["kaam"]      = {L, UserLanguage::Punglish};
    // Benglish
    sectionMap_["rochona"]   = {S, UserLanguage::Benglish};
    sectionMap_["rup"]       = {St,UserLanguage::Benglish};
    sectionMap_["kaj"]       = {L, UserLanguage::Benglish};
    // Gujlish
    sectionMap_["rachna"]    = {S, UserLanguage::Gujlish};
    sectionMap_["swarup"]    = {St,UserLanguage::Gujlish};
    sectionMap_["kaam"]      = {L, UserLanguage::Gujlish};
    // Marathlish
    sectionMap_["rachana"]   = {S, UserLanguage::Marathlish};
    sectionMap_["swaroop"]   = {St,UserLanguage::Marathlish};
    sectionMap_["kaarya"]    = {L, UserLanguage::Marathlish};
    // Telglish
    sectionMap_["amaipu"]    = {S, UserLanguage::Telglish};
    sectionMap_["roopa"]     = {St,UserLanguage::Telglish};
    sectionMap_["pani"]      = {L, UserLanguage::Telglish};
    // Kanglish
    sectionMap_["rachane"]   = {S, UserLanguage::Kanglish};
    sectionMap_["roopa"]     = {St,UserLanguage::Kanglish};
    sectionMap_["kelsa"]     = {L, UserLanguage::Kanglish};
    // Manglish
    sectionMap_["aakaaram"]  = {S, UserLanguage::Manglish};
    sectionMap_["roopam"]    = {St,UserLanguage::Manglish};
    sectionMap_["cheyu"]     = {L, UserLanguage::Manglish};
    // Odlish
    sectionMap_["kaakaathi"] = {S, UserLanguage::Odlish};
    sectionMap_["rupa"]      = {St,UserLanguage::Odlish};
    sectionMap_["kaam"]      = {L, UserLanguage::Odlish};
    // Urlish
    sectionMap_["dhanche"]   = {S, UserLanguage::Urlish};
    sectionMap_["roop"]      = {St,UserLanguage::Urlish};
    sectionMap_["kaam"]      = {L, UserLanguage::Urlish};
    // Asslish
    sectionMap_["aakar"]     = {S, UserLanguage::Asslish};
    sectionMap_["roop"]      = {St,UserLanguage::Asslish};
    sectionMap_["kaam"]      = {L, UserLanguage::Asslish};
    // Maithlish
    sectionMap_["rachana"]   = {S, UserLanguage::Maithlish};
    sectionMap_["roop"]      = {St,UserLanguage::Maithlish};
    sectionMap_["kaam"]      = {L, UserLanguage::Maithlish};
    // Konklish
    sectionMap_["rachana"]   = {S, UserLanguage::Konklish};
    sectionMap_["roop"]      = {St,UserLanguage::Konklish};
    sectionMap_["kaam"]      = {L, UserLanguage::Konklish};
    // Neplish
    sectionMap_["rachana"]   = {S, UserLanguage::Neplish};
    sectionMap_["roop"]      = {St,UserLanguage::Neplish};
    sectionMap_["kaam"]      = {L, UserLanguage::Neplish};
    // Sanglish
    sectionMap_["rachana"]   = {S, UserLanguage::Sanglish};
    sectionMap_["roop"]      = {St,UserLanguage::Sanglish};
    sectionMap_["karya"]     = {L, UserLanguage::Sanglish};
    // Kashlish
    sectionMap_["dhanche"]   = {S, UserLanguage::Kashlish};
    sectionMap_["roop"]      = {St,UserLanguage::Kashlish};
    sectionMap_["kaam"]      = {L, UserLanguage::Kashlish};
    // Sindlish
    sectionMap_["dhanche"]   = {S, UserLanguage::Sindlish};
    sectionMap_["roop"]      = {St,UserLanguage::Sindlish};
    sectionMap_["kaam"]      = {L, UserLanguage::Sindlish};
    // Doglish
    sectionMap_["dhanche"]   = {S, UserLanguage::Doglish};
    sectionMap_["roop"]      = {St,UserLanguage::Doglish};
    sectionMap_["kaam"]      = {L, UserLanguage::Doglish};
    // Manlish
    sectionMap_["sangai"]    = {S, UserLanguage::Manlish};
    sectionMap_["roop"]      = {St,UserLanguage::Manlish};
    sectionMap_["thabak"]    = {L, UserLanguage::Manlish};
    // Bodlish
    sectionMap_["rachana"]   = {S, UserLanguage::Bodlish};
    sectionMap_["roop"]      = {St,UserLanguage::Bodlish};
    sectionMap_["kaam"]      = {L, UserLanguage::Bodlish};
    // Santlish
    sectionMap_["rachana"]   = {S, UserLanguage::Santlish};
    sectionMap_["roop"]      = {St,UserLanguage::Santlish};
    sectionMap_["kaam"]      = {L, UserLanguage::Santlish};
    // English
    sectionMap_["structure"] = {S, UserLanguage::English};
    sectionMap_["style"]     = {St,UserLanguage::English};
    sectionMap_["logic"]     = {L, UserLanguage::English};

    // ---- HTML tag map (all 22 languages) ----
    // Headings
    for (auto& w : {"sheershak","sirshak","talaipu","matha","shirshak",
                    "talaippu","shirshaka","heading","h1"})
        tagMap_[w] = "h1";
    for (auto& w : {"upa_sheershak","subtitle","h2"})
        tagMap_[w] = "h2";
    // Paragraph
    for (auto& w : {"para","vakya","abzatz","vaakyam","paragraph","p"})
        tagMap_[w] = "p";
    // Button
    for (auto& w : {"button","batton","knoppu","botan","botama","button"})
        tagMap_[w] = "button";
    // Link/anchor
    for (auto& w : {"link","jodna","inaippu","iraipu","a","href"})
        tagMap_[w] = "a";
    // Image
    for (auto& w : {"chitransh","padham","image","chitra","img","tasviir"})
        tagMap_[w] = "img";
    // List
    for (auto& w : {"soochi","pattial","list","suchi","ul"})
        tagMap_[w] = "ul";
    // List item
    for (auto& w : {"ansh","item","li","tarat"})
        tagMap_[w] = "li";
    // Div container
    for (auto& w : {"khaana","section","khanda","div","container"})
        tagMap_[w] = "div";
    // Input
    for (auto& w : {"darj_karo","input","pravesh","nivedanam"})
        tagMap_[w] = "input";
    // Span
    for (auto& w : {"span","ansh_shabd"})
        tagMap_[w] = "span";
    // Navigation
    for (auto& w : {"nav","navigation","navi"})
        tagMap_[w] = "nav";
    // Footer
    for (auto& w : {"paad","footer","paada"})
        tagMap_[w] = "footer";
    // Header
    for (auto& w : {"header","siraa","upar"})
        tagMap_[w] = "header";

    // ---- CSS property map (all 22 languages) ----
    // Color
    for (auto& w : {"rang","niram","rong","ranga","vanna","barna","color","colour"})
        cssPropMap_[w] = "color";
    // Background color
    for (auto& w : {"prishtha_rang","pinnal_niram","background_rang",
                    "bg_rang","background","background-color","bg"})
        cssPropMap_[w] = "background-color";
    // Font size
    for (auto& w : {"pitha","alavai","font_size","akshar_maapha",
                    "font-size","pitha_maapha","size"})
        cssPropMap_[w] = "font-size";
    // Padding
    for (auto& w : {"haashiya","idam","padding","antariksha","antar"})
        cssPropMap_[w] = "padding";
    // Margin
    for (auto& w : {"margin","baahar","bahaar","margin_antar"})
        cssPropMap_[w] = "margin";
    // Border
    for (auto& w : {"seema","velai","border","had","hadh"})
        cssPropMap_[w] = "border";
    // Border radius
    for (auto& w : {"gol_kono","valaivu","radius","gol","golaai","rounded"})
        cssPropMap_[w] = "border-radius";
    // Width
    for (auto& w : {"chaurai","aagalam","width","chaudai"})
        cssPropMap_[w] = "width";
    // Height
    for (auto& w : {"unchai","uyaram","height","lambai_ud"})
        cssPropMap_[w] = "height";
    // Text align
    for (auto& w : {"madhya","center","align","text_align","text-align"})
        cssPropMap_[w] = "text-align";
    // Font weight
    for (auto& w : {"mota","bold","font_weight","font-weight"})
        cssPropMap_[w] = "font-weight";
    // Display
    for (auto& w : {"dikhana","display","show"})
        cssPropMap_[w] = "display";
    // Flex
    for (auto& w : {"flex","lachila","lachilapan"})
        cssPropMap_[w] = "display";
    // Cursor
    for (auto& w : {"cursor","kursor","maus"})
        cssPropMap_[w] = "cursor";
    // Opacity
    for (auto& w : {"paaradharshita","opacity","andhera"})
        cssPropMap_[w] = "opacity";

    // CSS value map
    cssValueMap_["naranga"]  = "#FF6B00";
    cssValueMap_["orange"]   = "#FF6B00";
    cssValueMap_["laal"]     = "red";
    cssValueMap_["red"]      = "red";
    cssValueMap_["neela"]    = "blue";
    cssValueMap_["blue"]     = "blue";
    cssValueMap_["hara"]     = "green";
    cssValueMap_["green"]    = "green";
    cssValueMap_["safed"]    = "white";
    cssValueMap_["white"]    = "white";
    cssValueMap_["kaala"]    = "black";
    cssValueMap_["black"]    = "black";
    cssValueMap_["peela"]    = "yellow";
    cssValueMap_["yellow"]   = "yellow";
    cssValueMap_["sach"]     = "center";
    cssValueMap_["madhya"]   = "center";
    cssValueMap_["none"]     = "none";
    cssValueMap_["nahi"]     = "none";
    cssValueMap_["mota"]     = "bold";

    // Event type map
    for (auto& w : {"click_pe","ondabu","klik","onclick","click","dabane_par"})
        eventTypeMap_[w] = "click";
    for (auto& w : {"hover_pe","onhover","hover","upar_aane_par"})
        eventTypeMap_[w] = "mouseover";
    for (auto& w : {"load_pe","onload","load","kholne_par"})
        eventTypeMap_[w] = "load";

    // Action map
    for (auto& w : {"dikhao","kaado","dakho","dekhao","batavo","dakhav",
                    "chupinchu","toro","kaaniku","print","alert","sandesh"})
        actionMap_[w] = "alert";
    for (auto& w : {"jao","navigate","jaao","link_kholo","open"})
        actionMap_[w] = "navigate";
    for (auto& w : {"badlo","change","update","replace"})
        actionMap_[w] = "change";
    for (auto& w : {"console","log","console_log"})
        actionMap_[w] = "console.log";
}

std::string MtrParser::Trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::vector<std::string> MtrParser::SplitLines(const std::string& s) {
    std::vector<std::string> lines;
    std::istringstream ss(s);
    std::string line;
    while (std::getline(ss, line)) {
        if (line.size() && line.back() == '\r')
            line.pop_back();
        lines.push_back(line);
    }
    return lines;
}

int MtrParser::CountIndent(const std::string& line) {
    int count = 0;
    for (char c : line) {
        if (c == ' ') count++;
        else if (c == '\t') count += 2;
        else break;
    }
    return count;
}

MtrParser::SectionType MtrParser::IdentifySection(
    const std::string& name, UserLanguage& lang)
{
    EnsureMapsInitialized();
    std::string lower = name;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    auto it = sectionMap_.find(lower);
    if (it != sectionMap_.end()) {
        lang = it->second.second;
        return it->second.first;
    }
    return SectionType::None;
}

std::string MtrParser::TranslateTag(const std::string& name) const {
    std::string lower = name;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    auto it = tagMap_.find(lower);
    if (it != tagMap_.end()) return it->second;
    return "div"; // default
}

std::string MtrParser::TranslateCssProperty(const std::string& prop) const {
    std::string lower = prop;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    auto it = cssPropMap_.find(lower);
    if (it != cssPropMap_.end()) return it->second;
    return prop; // pass through
}

std::string MtrParser::TranslateCssValue(
    const std::string&, const std::string& val) const
{
    std::string lower = val;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    // Remove surrounding quotes
    if (lower.size() >= 2 &&
        ((lower.front() == '"' && lower.back() == '"') ||
         (lower.front() == '\'' && lower.back() == '\'')))
        lower = lower.substr(1, lower.size() - 2);
    auto it = cssValueMap_.find(lower);
    if (it != cssValueMap_.end()) return it->second;
    return val; // pass through
}

std::string MtrParser::TranslateEventType(const std::string& ev) const {
    std::string lower = ev;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    auto it = eventTypeMap_.find(lower);
    if (it != eventTypeMap_.end()) return it->second;
    return "click";
}

std::string MtrParser::TranslateAction(const std::string& act) const {
    std::string lower = act;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    auto it = actionMap_.find(lower);
    if (it != actionMap_.end()) return it->second;
    return "alert";
}

void MtrParser::ParseStructure(const std::string& content, MtrDocument& doc) {
    auto lines = SplitLines(content);
    int elem_id = 0;
    for (const auto& line : lines) {
        std::string t = Trim(line);
        if (t.empty() || t[0] == '#') continue;

        // Check for element = "content" pattern
        auto eq_pos = t.find('=');
        if (eq_pos != std::string::npos) {
            std::string elem_name = Trim(t.substr(0, eq_pos));
            std::string elem_val  = Trim(t.substr(eq_pos + 1));
            // Remove quotes
            if (elem_val.size() >= 2 && elem_val.front() == '"')
                elem_val = elem_val.substr(1, elem_val.size() - 2);

            MtrElement el;
            el.type    = TranslateTag(elem_name);
            el.content = elem_val;
            el.id      = elem_name + "_" + std::to_string(++elem_id);
            doc.elements.push_back(el);
        } else {
            // Element without content
            MtrElement el;
            el.type = TranslateTag(t);
            el.id   = t + "_" + std::to_string(++elem_id);
            doc.elements.push_back(el);
        }
    }
}

void MtrParser::ParseStyle(const std::string& content, MtrDocument& doc) {
    auto lines = SplitLines(content);
    std::string current_selector;
    int base_indent = -1;

    for (const auto& line : lines) {
        if (Trim(line).empty() || Trim(line)[0] == '#') continue;
        int indent = CountIndent(line);
        std::string t = Trim(line);

        if (base_indent < 0 || indent == 0) {
            // Top-level: this is a selector
            current_selector = t;
            base_indent = 0;
        } else {
            // Property: value
            auto eq = t.find('=');
            if (eq != std::string::npos) {
                std::string prop = Trim(t.substr(0, eq));
                std::string val  = Trim(t.substr(eq + 1));
                std::string css_prop = TranslateCssProperty(prop);
                std::string css_val  = TranslateCssValue(prop, val);
                doc.styles[current_selector][css_prop] = css_val;
            }
        }
    }
}

void MtrParser::ParseLogic(const std::string& content, MtrDocument& doc) {
    auto lines = SplitLines(content);
    std::string current_element;
    std::string current_event;

    for (const auto& line : lines) {
        std::string t = Trim(line);
        if (t.empty() || t[0] == '#') continue;

        // element.event pattern
        auto dot = t.find('.');
        if (dot != std::string::npos && t.find('=') == std::string::npos) {
            current_element = Trim(t.substr(0, dot));
            current_event   = Trim(t.substr(dot + 1));
        } else {
            // Action line: dikhao = "value"
            auto eq = t.find('=');
            if (eq != std::string::npos && !current_element.empty()) {
                std::string action_name = Trim(t.substr(0, eq));
                std::string action_val  = Trim(t.substr(eq + 1));
                if (action_val.size() >= 2 && action_val.front() == '"')
                    action_val = action_val.substr(1, action_val.size()-2);

                MtrEvent ev;
                ev.element_id = current_element;
                ev.event_type = TranslateEventType(current_event);
                ev.action     = TranslateAction(action_name);
                ev.value      = action_val;
                doc.events.push_back(ev);
            }
        }
    }
}

MtrDocument MtrParser::Parse(const std::string& source) {
    EnsureMapsInitialized();
    MtrDocument doc;

    // Split into sections
    auto lines = SplitLines(source);
    std::string current_section_content;
    SectionType current_section = SectionType::None;
    UserLanguage detected = UserLanguage::Unknown;

    for (size_t i = 0; i < lines.size(); ++i) {
        std::string t = Trim(lines[i]);

        // Section header: [section_name]
        if (t.size() >= 3 && t.front() == '[' && t.back() == ']') {
            // Process previous section
            if (current_section == SectionType::Structure)
                ParseStructure(current_section_content, doc);
            else if (current_section == SectionType::Style)
                ParseStyle(current_section_content, doc);
            else if (current_section == SectionType::Logic)
                ParseLogic(current_section_content, doc);

            current_section_content.clear();
            std::string sec_name = t.substr(1, t.size() - 2);
            UserLanguage lang;
            current_section = IdentifySection(sec_name, lang);
            if (detected == UserLanguage::Unknown)
                detected = lang;
        } else {
            current_section_content += lines[i] + "\n";
        }
    }

    // Process last section
    if (current_section == SectionType::Structure)
        ParseStructure(current_section_content, doc);
    else if (current_section == SectionType::Style)
        ParseStyle(current_section_content, doc);
    else if (current_section == SectionType::Logic)
        ParseLogic(current_section_content, doc);

    doc.detected_language = detected;
    return doc;
}

MtrDocument MtrParser::ParseFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filepath);
    std::stringstream buf;
    buf << file.rdbuf();
    return Parse(buf.str());
}

} // namespace mantra
