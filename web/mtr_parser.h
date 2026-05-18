#pragma once

#include <string>
#include <unordered_map>

namespace mantra::web {

struct MtrDocument {
    std::string structure;
    std::string style;
    std::string logic;
};

class MtrParser {
public:
    MtrDocument parse(const std::string& source) const;
};

} // namespace mantra::web
