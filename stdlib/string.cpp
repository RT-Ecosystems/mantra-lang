#include "string.h"

#include "../error/error.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

namespace mantra::stdlib {
namespace {

void ensureArgs(const std::vector<MantraValue>& args, size_t expected, const std::string& name) {
    if (args.size() != expected) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, name + " requires " + std::to_string(expected) + " args", 0, 0);
        throw std::runtime_error(name);
    }
}

std::string requireString(const MantraValue& value, const std::string& name) {
    if (value.type != ValueType::String) {
        ErrorHandler::printError(ErrorType::TYPE_ERROR, name + " expects string", 0, 0);
        throw std::runtime_error(name);
    }
    return value.string_value;
}

double requireNumber(const MantraValue& value, const std::string& name) {
    if (value.type != ValueType::Number) {
        ErrorHandler::printError(ErrorType::TYPE_ERROR, name + " expects number", 0, 0);
        throw std::runtime_error(name);
    }
    return value.number_value;
}

std::vector<size_t> utf8Offsets(const std::string& text) {
    std::vector<size_t> offsets;
    for (size_t i = 0; i < text.size();) {
        offsets.push_back(i);
        unsigned char ch = static_cast<unsigned char>(text[i]);
        if ((ch & 0x80u) == 0) i += 1;
        else if ((ch & 0xE0u) == 0xC0u) i += 2;
        else if ((ch & 0xF0u) == 0xE0u) i += 3;
        else if ((ch & 0xF8u) == 0xF0u) i += 4;
        else i += 1;
    }
    offsets.push_back(text.size());
    return offsets;
}

size_t utf8ByteLength(unsigned char ch) {
    if ((ch & 0x80u) == 0) return 1;
    if ((ch & 0xE0u) == 0xC0u) return 2;
    if ((ch & 0xF0u) == 0xE0u) return 3;
    return 4;
}

std::string utf8Slice(const std::string& text, long start, long end) {
    auto offsets = utf8Offsets(text);
    long size = static_cast<long>(offsets.size()) - 1;
    if (start < 0) start += size;
    if (end < 0) end += size;
    start = std::clamp(start, 0L, size);
    end = std::clamp(end, 0L, size);
    if (end < start) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, "slice end must be >= start", 0, 0);
        throw std::runtime_error("slice");
    }
    return text.substr(offsets[static_cast<size_t>(start)], offsets[static_cast<size_t>(end)] - offsets[static_cast<size_t>(start)]);
}

std::string asciiLower(std::string s) {
    for (char& ch : s) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return s;
}

std::string asciiUpper(std::string s) {
    for (char& ch : s) {
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    return s;
}

bool startsWith(const std::string& text, const std::string& prefix) {
    return text.rfind(prefix, 0) == 0;
}

bool endsWith(const std::string& text, const std::string& suffix) {
    return text.size() >= suffix.size() &&
           text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
}

} // namespace

MantraValue builtinUpper(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "upper");
    return MantraValue::string(asciiUpper(requireString(args[0], "upper")));
}

MantraValue builtinLower(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "lower");
    return MantraValue::string(asciiLower(requireString(args[0], "lower")));
}

MantraValue builtinTrim(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "trim");
    std::string s = requireString(args[0], "trim");
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return MantraValue::string("");
    }
    size_t end = s.find_last_not_of(" \t\r\n");
    return MantraValue::string(s.substr(start, end - start + 1));
}

MantraValue builtinSplit(const std::vector<MantraValue>& args) {
    ensureArgs(args, 2, "split");
    std::string s = requireString(args[0], "split");
    std::string delim = requireString(args[1], "split");
    std::vector<MantraValue> parts;
    if (delim.empty()) {
        for (size_t i = 0; i < s.size();) {
            unsigned char ch = static_cast<unsigned char>(s[i]);
            size_t len = utf8ByteLength(ch);
            parts.push_back(MantraValue::string(s.substr(i, len)));
            i += len;
        }
        return MantraValue::array(std::move(parts));
    }
    size_t pos = 0;
    while (true) {
        size_t next = s.find(delim, pos);
        parts.push_back(MantraValue::string(s.substr(pos, next == std::string::npos ? std::string::npos : next - pos)));
        if (next == std::string::npos) break;
        pos = next + delim.size();
    }
    return MantraValue::array(std::move(parts));
}

MantraValue builtinReplace(const std::vector<MantraValue>& args) {
    ensureArgs(args, 3, "replace");
    std::string s = requireString(args[0], "replace");
    std::string oldv = requireString(args[1], "replace");
    std::string newv = requireString(args[2], "replace");
    if (oldv.empty()) return MantraValue::string(s);
    size_t pos = 0;
    while ((pos = s.find(oldv, pos)) != std::string::npos) {
        s.replace(pos, oldv.size(), newv);
        pos += newv.size();
    }
    return MantraValue::string(s);
}

MantraValue builtinSlice(const std::vector<MantraValue>& args) {
    if (args.size() < 2 || args.size() > 3) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, "slice expects 2 or 3 args", 0, 0);
        throw std::runtime_error("slice");
    }
    std::string s = requireString(args[0], "slice");
    long start = static_cast<long>(requireNumber(args[1], "slice"));
    long end = args.size() == 3 ? static_cast<long>(requireNumber(args[2], "slice"))
                                : static_cast<long>(utf8Offsets(s).size() - 1);
    return MantraValue::string(utf8Slice(s, start, end));
}

MantraValue builtinContains(const std::vector<MantraValue>& args) {
    ensureArgs(args, 2, "contains");
    return MantraValue::boolean(requireString(args[0], "contains").find(requireString(args[1], "contains")) != std::string::npos);
}

MantraValue builtinStartsWith(const std::vector<MantraValue>& args) {
    ensureArgs(args, 2, "startsWith");
    return MantraValue::boolean(startsWith(requireString(args[0], "startsWith"), requireString(args[1], "startsWith")));
}

MantraValue builtinEndsWith(const std::vector<MantraValue>& args) {
    ensureArgs(args, 2, "endsWith");
    return MantraValue::boolean(endsWith(requireString(args[0], "endsWith"), requireString(args[1], "endsWith")));
}

MantraValue builtinRepeat(const std::vector<MantraValue>& args) {
    ensureArgs(args, 2, "repeat");
    std::string s = requireString(args[0], "repeat");
    int count = static_cast<int>(requireNumber(args[1], "repeat"));
    if (count < 0) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, "repeat count must be non-negative", 0, 0);
        throw std::runtime_error("repeat");
    }
    std::string out;
    out.reserve(s.size() * static_cast<size_t>(count));
    for (int i = 0; i < count; ++i) out += s;
    return MantraValue::string(out);
}

} // namespace mantra::stdlib
