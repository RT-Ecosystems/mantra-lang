#pragma once

#include "mtr_parser.h"

#include <string>

namespace mantra::web {

class HtmlGenerator {
public:
    std::string generate(const MtrDocument& document) const;
};

} // namespace mantra::web
