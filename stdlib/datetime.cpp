#include "datetime.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
namespace mantra::stdlib {
MantraValue builtinNow(const std::vector<MantraValue>&) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return MantraValue::string(ss.str());
}
MantraValue builtinToday(const std::vector<MantraValue>&) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d");
    return MantraValue::string(ss.str());
}
}
