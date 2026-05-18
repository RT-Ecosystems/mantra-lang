#include "io.h"
#include "../error/error.h"
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace mantra::stdlib {

namespace {

constexpr int kDefaultExitCode = 1;
constexpr size_t kMaxArrayPrintDepth = 8;

void raiseError(ErrorType type, const std::string& message) {
    ErrorHandler::printError(type, message, 0, 0);
    throw std::runtime_error(message);
}

void ensureArgCount(const std::vector<MantraValue>& args,
                    size_t expected,
                    const std::string& name) {
    if (args.size() != expected) {
        raiseError(ErrorType::RUNTIME_ERROR,
                   name + " को " + std::to_string(expected) + " आर्ग्युमेंट चाहिए");
    }
}

std::string requireString(const MantraValue& value, const std::string& name) {
    if (value.type != ValueType::String) {
        raiseError(ErrorType::TYPE_ERROR, name + " के लिए string अपेक्षित है");
    }
    return value.string_value;
}

std::string trim(const std::string& input) {
    size_t start = 0;
    while (start < input.size() && std::isspace(static_cast<unsigned char>(input[start]))) {
        start++;
    }
    size_t end = input.size();
    while (end > start && std::isspace(static_cast<unsigned char>(input[end - 1]))) {
        end--;
    }
    return input.substr(start, end - start);
}

bool isWhitespaceOnly(const std::string& input) {
    for (char ch : input) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            return false;
        }
    }
    return true;
}

bool isNumericString(const std::string& input) {
    if (input.empty()) {
        return false;
    }
    size_t idx = 0;
    if (input[idx] == '+' || input[idx] == '-') {
        idx++;
    }
    bool saw_digit = false;
    bool saw_dot = false;
    while (idx < input.size()) {
        char ch = input[idx];
        if (std::isdigit(static_cast<unsigned char>(ch))) {
            saw_digit = true;
            idx++;
            continue;
        }
        if (ch == '.' && !saw_dot) {
            saw_dot = true;
            idx++;
            continue;
        }
        break;
    }
    return saw_digit;
}

size_t countUtf8Codepoints(const std::string& text) {
    size_t count = 0;
    size_t i = 0;
    while (i < text.size()) {
        unsigned char ch = static_cast<unsigned char>(text[i]);
        if ((ch & 0x80u) == 0) {
            i += 1;
        } else if ((ch & 0xE0u) == 0xC0u) {
            i += 2;
        } else if ((ch & 0xF0u) == 0xE0u) {
            i += 3;
        } else if ((ch & 0xF8u) == 0xF0u) {
            i += 4;
        } else {
            i += 1;
        }
        count++;
    }
    return count;
}

bool isFiniteNumber(double value) {
    return std::isfinite(value);
}

double normalizeZero(double value) {
    if (std::abs(value) < 1e-12) {
        return 0.0;
    }
    return value;
}

std::string formatNumber(double value) {
    if (!isFiniteNumber(value)) {
        if (std::isnan(value)) {
            return "nan";
        }
        return value > 0 ? "inf" : "-inf";
    }

    value = normalizeZero(value);

    std::ostringstream out;
    out << std::setprecision(15) << value;
    std::string text = out.str();
    if (text.find('.') != std::string::npos) {
        while (!text.empty() && text.back() == '0') {
            text.pop_back();
        }
        if (!text.empty() && text.back() == '.') {
            text.pop_back();
        }
    }
    if (text.empty()) {
        return "0";
    }
    return text;
}

std::string formatBoolean(bool value) {
    return value ? "true" : "false";
}

std::string formatNull() {
    return "null";
}

std::string formatFunction(const MantraValue& value) {
    if (value.function && !value.function->name.empty()) {
        return "<function " + value.function->name + ">";
    }
    return "<function>";
}

std::string formatString(const std::string& value) {
    return value;
}

std::string formatValue(const MantraValue& value, size_t depth);

std::string formatArray(const std::vector<MantraValue>& values, size_t depth) {
    if (depth >= kMaxArrayPrintDepth) {
        return "[...]";
    }
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        out << formatValue(values[i], depth + 1);
        if (i + 1 < values.size()) {
            out << ", ";
        }
    }
    out << "]";
    return out.str();
}

std::string formatValue(const MantraValue& value, size_t depth) {
    switch (value.type) {
        case ValueType::Number:
            return formatNumber(value.number_value);
        case ValueType::String:
            return formatString(value.string_value);
        case ValueType::Boolean:
            return formatBoolean(value.bool_value);
        case ValueType::Null:
            return formatNull();
        case ValueType::Function:
            return formatFunction(value);
        case ValueType::Array:
            return formatArray(value.array_value, depth);
        default:
            return "<unknown>";
    }
}

MantraValue makeStringValue(const std::string& text) {
    return MantraValue::string(text);
}

MantraValue makeNumberValue(double value) {
    return MantraValue::number(value);
}

double parseNumberString(const std::string& text, const std::string& name) {
    if (isWhitespaceOnly(text)) {
        raiseError(ErrorType::RUNTIME_ERROR, name + " में खाली string नहीं चलेगी");
    }

    std::string trimmed = trim(text);
    if (trimmed.empty()) {
        raiseError(ErrorType::RUNTIME_ERROR, name + " में खाली string नहीं चलेगी");
    }
    if (!isNumericString(trimmed)) {
        raiseError(ErrorType::RUNTIME_ERROR, name + " में अमान्य string: " + text);
    }

    try {
        size_t idx = 0;
        double result = std::stod(trimmed, &idx);
        if (idx != trimmed.size()) {
            raiseError(ErrorType::RUNTIME_ERROR, name + " में अमान्य string: " + text);
        }
        return result;
    } catch (const std::exception&) {
        raiseError(ErrorType::RUNTIME_ERROR, name + " में अमान्य string: " + text);
    }

    return 0.0;
}

MantraValue parseNumberValue(const MantraValue& value, const std::string& name) {
    if (value.type == ValueType::Number) {
        return makeNumberValue(value.number_value);
    }
    if (value.type == ValueType::Boolean) {
        return makeNumberValue(value.bool_value ? 1.0 : 0.0);
    }
    if (value.type == ValueType::String) {
        return makeNumberValue(parseNumberString(value.string_value, name));
    }
    raiseError(ErrorType::TYPE_ERROR, name + " के लिए string या number अपेक्षित है");
    return MantraValue::nullValue();
}

bool isPrintable(const std::vector<MantraValue>& args) {
    (void)args;
    return true;
}

void appendPrintable(std::ostringstream& out, const MantraValue& value) {
    out << formatValue(value, 0);
}

std::string normalizeSeparator(const std::string& separator) {
    if (separator.empty()) {
        return " ";
    }
    return separator;
}

std::string joinPrintable(const std::vector<MantraValue>& args, const std::string& separator) {
    std::string sep = normalizeSeparator(separator);
    std::ostringstream out;
    for (size_t i = 0; i < args.size(); ++i) {
        appendPrintable(out, args[i]);
        if (i + 1 < args.size()) {
            out << sep;
        }
    }
    return out.str();
}

std::string joinPrintable(const std::vector<MantraValue>& args) {
    return joinPrintable(args, " ");
}

std::string buildPrompt(const std::vector<MantraValue>& args) {
    if (args.empty()) {
        return "";
    }
    return joinPrintable(args, " ");
}

std::string buildOutputLine(const std::vector<MantraValue>& args) {
    if (args.empty()) {
        return "";
    }
    return joinPrintable(args);
}

void printLine(const std::string& text) {
    std::cout << text << std::endl;
    std::cout.flush();
}

} // namespace

MantraValue builtinPrint(const std::vector<MantraValue>& args) {
    if (!isPrintable(args)) {
        raiseError(ErrorType::TYPE_ERROR, "print के लिए मान्य वैल्यू अपेक्षित है");
    }
    printLine(buildOutputLine(args));
    return MantraValue::nullValue();
}

MantraValue builtinInput(const std::vector<MantraValue>& args) {
    std::string prompt = buildPrompt(args);
    if (!prompt.empty()) {
        std::cout << prompt;
        std::cout.flush();
    }

    std::string line;
    std::getline(std::cin, line);
    return makeStringValue(line);
}

MantraValue builtinLength(const std::vector<MantraValue>& args) {
    ensureArgCount(args, 1, "lambai/length");
    const auto& value = args.front();

    if (value.type == ValueType::String) {
        return makeNumberValue(static_cast<double>(countUtf8Codepoints(value.string_value)));
    }

    if (value.type == ValueType::Array) {
        return makeNumberValue(static_cast<double>(value.array_value.size()));
    }

    raiseError(ErrorType::TYPE_ERROR, "lambai/length केवल string या array पर चलेगा");
    return MantraValue::nullValue();
}

MantraValue builtinConcat(const std::vector<MantraValue>& args) {
    ensureArgCount(args, 2, "jodo_shabd/concat");

    std::string left = requireString(args[0], "jodo_shabd/concat");
    std::string right = requireString(args[1], "jodo_shabd/concat");

    return makeStringValue(left + right);
}

MantraValue builtinToNumber(const std::vector<MantraValue>& args) {
    ensureArgCount(args, 1, "sankhya/toNumber");
    return parseNumberValue(args[0], "sankhya/toNumber");
}

MantraValue builtinToString(const std::vector<MantraValue>& args) {
    ensureArgCount(args, 1, "shabd/toString");
    return makeStringValue(formatValue(args[0], 0));
}

MantraValue builtinType(const std::vector<MantraValue>& args) {
    ensureArgCount(args, 1, "type");
    return makeStringValue(args[0].typeName());
}

MantraValue builtinExit(const std::vector<MantraValue>& args) {
    if (!args.empty()) {
        double code = 0.0;
        if (args[0].type == ValueType::Number) {
            code = args[0].number_value;
        }
        std::exit(static_cast<int>(code));
    }
    std::exit(0);
}

MantraValue builtinClear(const std::vector<MantraValue>& args) {
    (void)args;
    std::cout << "\033[2J\033[H";
    std::cout.flush();
    return MantraValue::nullValue();
}














































} // namespace mantra::stdlib
