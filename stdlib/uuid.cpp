#include "uuid.h"
#include <random>
#include <sstream>
#include <iomanip>
namespace mantra::stdlib {
MantraValue builtinUUID(const std::vector<MantraValue>&) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        if (i == 8 || i == 12 || i == 16 || i == 20) ss << '-';
        int r = dis(gen);
        if (i == 12) r = 4;
        if (i == 16) r = (r & 0x3) | 0x8;
        ss << std::hex << r;
    }
    return MantraValue::string(ss.str());
}
}
