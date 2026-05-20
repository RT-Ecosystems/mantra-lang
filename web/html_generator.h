#pragma once
#include "mtr_parser.h"
#include <string>

namespace mantra {

class HtmlGenerator {
public:
    HtmlGenerator();
    std::string Generate(const MtrDocument& doc,
                         const std::string& title = "MANTRA Website");
    bool GenerateFile(const MtrDocument& doc,
                      const std::string& output_path,
                      const std::string& title = "MANTRA Website");

private:
    std::string GenerateHtml(const MtrDocument& doc) const;
    std::string GenerateCss(const MtrDocument& doc)  const;
    std::string GenerateJs(const MtrDocument& doc)   const;
    std::string RenderElement(const MtrElement& el)  const;
    std::string EscapeHtml(const std::string& s)     const;
    std::string MakeSafeId(const std::string& s)     const;
};

} // namespace mantra
