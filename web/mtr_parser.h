#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mantra/core/types.h>

namespace mantra {

// UserLanguage is now defined in mantra/core/types.h
using UserLanguage = mantra::UserLanguage;

struct MtrElement {
    std::string type;       // HTML tag: h1, p, button, a, img, ul, div
    std::string content;    // inner text
    std::string id;         // CSS selector / JS target
    std::vector<std::pair<std::string,std::string>> attributes;
    std::vector<MtrElement> children;
};

struct MtrEvent {
    std::string element_id;
    std::string event_type; // click, hover, etc.
    std::string action;     // alert, navigate, etc.
    std::string value;
};

struct MtrDocument {
    std::vector<MtrElement> elements;
    std::map<std::string,std::map<std::string,std::string>> styles;
    std::vector<MtrEvent> events;
    UserLanguage detected_language = UserLanguage::Unknown;
    std::string page_title;
};

class MtrParser {
public:
    MtrParser();
    MtrDocument Parse(const std::string& source);
    MtrDocument ParseFile(const std::string& filepath);

private:
    enum class SectionType { Structure, Style, Logic, None };

    void EnsureMapsInitialized();
    SectionType IdentifySection(const std::string& name, UserLanguage& lang);

    static std::string Trim(const std::string& s);
    static std::vector<std::string> SplitLines(const std::string& s);
    static int CountIndent(const std::string& line);

    void ParseStructure(const std::string& content, MtrDocument& doc);
    void ParseStyle(const std::string& content, MtrDocument& doc);
    void ParseLogic(const std::string& content, MtrDocument& doc);

    std::string TranslateTag(const std::string& name) const;
    std::string TranslateCssProperty(const std::string& prop) const;
    std::string TranslateCssValue(const std::string& prop,
                                   const std::string& val) const;
    std::string TranslateEventType(const std::string& ev) const;
    std::string TranslateAction(const std::string& act) const;

    std::map<std::string,std::pair<SectionType,UserLanguage>> sectionMap_;
    std::map<std::string,std::string> tagMap_;
    std::map<std::string,std::string> cssPropMap_;
    std::map<std::string,std::string> cssValueMap_;
    std::map<std::string,std::string> eventTypeMap_;
    std::map<std::string,std::string> actionMap_;
    bool mapsInitialized_ = false;
};

} // namespace mantra
