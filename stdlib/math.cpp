#include "math.h"

#include "../error/error.h"

#include <cmath>
#include <random>
#include <limits>
#include <stdexcept>

namespace mantra::stdlib {
namespace {

void ensureArgs(const std::vector<MantraValue>& args, size_t expected, const std::string& name) {
    if (args.size() != expected) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, name + " expects " + std::to_string(expected) + " args", 0, 0);
        throw std::runtime_error(name);
    }
}

double requireNumber(const MantraValue& value, const std::string& name) {
    if (value.type != ValueType::Number) {
        ErrorHandler::printError(ErrorType::TYPE_ERROR, name + " expects number", 0, 0);
        throw std::runtime_error(name);
    }
    return value.number_value;
}

MantraValue unaryMath(const std::vector<MantraValue>& args, const std::string& name, double (*fn)(double)) {
    ensureArgs(args, 1, name);
    return MantraValue::number(fn(requireNumber(args[0], name)));
}

} // namespace

MantraValue builtinSin(const std::vector<MantraValue>& args) { return unaryMath(args, "sin", std::sin); }
MantraValue builtinCos(const std::vector<MantraValue>& args) { return unaryMath(args, "cos", std::cos); }
MantraValue builtinTan(const std::vector<MantraValue>& args) { return unaryMath(args, "tan", std::tan); }
MantraValue builtinAsin(const std::vector<MantraValue>& args) { return unaryMath(args, "asin", std::asin); }
MantraValue builtinAcos(const std::vector<MantraValue>& args) { return unaryMath(args, "acos", std::acos); }
MantraValue builtinAtan(const std::vector<MantraValue>& args) { return unaryMath(args, "atan", std::atan); }
MantraValue builtinLog(const std::vector<MantraValue>& args) { return unaryMath(args, "log", std::log); }
MantraValue builtinLog10(const std::vector<MantraValue>& args) { return unaryMath(args, "log10", std::log10); }
MantraValue builtinExp(const std::vector<MantraValue>& args) { return unaryMath(args, "exp", std::exp); }

MantraValue builtinPow(const std::vector<MantraValue>& args) {
    ensureArgs(args, 2, "pow");
    return MantraValue::number(std::pow(requireNumber(args[0], "pow"), requireNumber(args[1], "pow")));
}

MantraValue builtinAbs(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "abs");
    return MantraValue::number(std::fabs(requireNumber(args[0], "abs")));
}

MantraValue builtinSqrt(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "sqrt");
    return MantraValue::number(std::sqrt(requireNumber(args[0], "sqrt")));
}

MantraValue builtinRound(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "round");
    return MantraValue::number(std::round(requireNumber(args[0], "round")));
}

MantraValue builtinCeil(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "ceil");
    return MantraValue::number(std::ceil(requireNumber(args[0], "ceil")));
}

MantraValue builtinFloor(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "floor");
    return MantraValue::number(std::floor(requireNumber(args[0], "floor")));
}

MantraValue builtinRandom(const std::vector<MantraValue>& args) {
    if (!args.empty()) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, "random expects no args", 0, 0);
        throw std::runtime_error("random");
    }
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return MantraValue::number(dist(gen));
}

MantraValue builtinPi(const std::vector<MantraValue>& args) {
    if (!args.empty()) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, "pi expects no args", 0, 0);
        throw std::runtime_error("pi");
    }
    return MantraValue::number(3.14159265358979323846);
}

MantraValue builtinE(const std::vector<MantraValue>& args) {
    if (!args.empty()) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, "e expects no args", 0, 0);
        throw std::runtime_error("e");
    }
    return MantraValue::number(2.71828182845904523536);
}

MantraValue builtinInfinity(const std::vector<MantraValue>& args) {
    if (!args.empty()) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, "infinity expects no args", 0, 0);
        throw std::runtime_error("infinity");
    }
    return MantraValue::number(std::numeric_limits<double>::infinity());
}

MantraValue builtinNaN(const std::vector<MantraValue>& args) {
    if (!args.empty()) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, "nan expects no args", 0, 0);
        throw std::runtime_error("nan");
    }
    return MantraValue::number(std::numeric_limits<double>::quiet_NaN());
}

MantraValue builtinIsInfinity(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "is_infinity");
    return MantraValue::boolean(std::isinf(requireNumber(args[0], "is_infinity")));
}

MantraValue builtinIsNaN(const std::vector<MantraValue>& args) {
    ensureArgs(args, 1, "is_nan");
    return MantraValue::boolean(std::isnan(requireNumber(args[0], "is_nan")));
}

} // namespace mantra::stdlib
