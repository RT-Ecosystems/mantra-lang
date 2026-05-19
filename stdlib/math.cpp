#include "math.h"

#include "../error/error.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace mantra::stdlib {
namespace {

void ensureArgs(const std::vector<MantraValue>& args, size_t expected, const std::string& name) {
    if (args.size() != expected) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR,
                                 name + " requires " + std::to_string(expected) + " args",
                                 0, 0);
        throw std::runtime_error(name);
    }
}

void ensureAtLeast(const std::vector<MantraValue>& args, size_t minimum, const std::string& name) {
    if (args.size() < minimum) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR,
                                 name + " requires at least " + std::to_string(minimum) + " args",
                                 0, 0);
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

std::vector<double> collectNumbers(const std::vector<MantraValue>& args, const std::string& name) {
    std::vector<double> values;
    if (args.size() == 1 && args[0].type == ValueType::Array) {
        values.reserve(args[0].array_value.size());
        for (const auto& item : args[0].array_value) {
            values.push_back(requireNumber(item, name));
        }
        return values;
    }

    values.reserve(args.size());
    for (const auto& arg : args) {
        values.push_back(requireNumber(arg, name));
    }
    return values;
}

double sumValues(const std::vector<double>& values) {
    double total = 0.0;
    for (double value : values) {
        total += value;
    }
    return total;
}

MantraValue unaryMath(const std::vector<MantraValue>& args, const std::string& name, double (*fn)(double)) {
    ensureArgs(args, 1, name);
    return MantraValue::number(fn(requireNumber(args[0], name)));
}

std::vector<double> requireSeries(const std::vector<MantraValue>& args, const std::string& name) {
    ensureAtLeast(args, 1, name);
    std::vector<double> values = collectNumbers(args, name);
    if (values.empty()) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, name + " requires at least one number", 0, 0);
        throw std::runtime_error(name);
    }
    return values;
}

double meanOf(const std::vector<double>& values) {
    return sumValues(values) / static_cast<double>(values.size());
}

double medianOf(std::vector<double> values) {
    std::sort(values.begin(), values.end());
    const size_t mid = values.size() / 2;
    if (values.size() % 2 == 1) {
        return values[mid];
    }
    return (values[mid - 1] + values[mid]) / 2.0;
}

double modeOf(const std::vector<double>& values) {
    std::unordered_map<double, std::pair<size_t, size_t>> counts;
    counts.reserve(values.size());
    size_t best_count = 0;
    size_t best_first_index = values.size();
    double best_value = values[0];

    for (size_t i = 0; i < values.size(); ++i) {
        const double value = values[i];
        auto& entry = counts[value];
        if (entry.first == 0) {
            entry.second = i;
        }
        ++entry.first;
        if (entry.first > best_count || (entry.first == best_count && entry.second < best_first_index)) {
            best_count = entry.first;
            best_first_index = entry.second;
            best_value = value;
        }
    }

    return best_value;
}

double populationStdDev(const std::vector<double>& values) {
    const double mean = meanOf(values);
    double variance = 0.0;
    for (double value : values) {
        const double diff = value - mean;
        variance += diff * diff;
    }
    variance /= static_cast<double>(values.size());
    return std::sqrt(variance);
}

double maxOf(const std::vector<double>& values) {
    return *std::max_element(values.begin(), values.end());
}

double minOf(const std::vector<double>& values) {
    return *std::min_element(values.begin(), values.end());
}

} // namespace

MantraValue builtinAdd(const std::vector<MantraValue>& args) {
    std::vector<double> values = collectNumbers(args, "jodo");
    return MantraValue::number(sumValues(values));
}

MantraValue builtinSubtract(const std::vector<MantraValue>& args) {
    ensureAtLeast(args, 2, "ghato");
    std::vector<double> values = collectNumbers(args, "ghato");
    if (values.size() < 2) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, "ghato requires at least 2 numbers", 0, 0);
        throw std::runtime_error("ghato");
    }
    double result = values[0];
    for (size_t i = 1; i < values.size(); ++i) {
        result -= values[i];
    }
    return MantraValue::number(result);
}

MantraValue builtinMultiply(const std::vector<MantraValue>& args) {
    std::vector<double> values = collectNumbers(args, "guno");
    double result = 1.0;
    for (double value : values) {
        result *= value;
    }
    return MantraValue::number(result);
}

MantraValue builtinDivide(const std::vector<MantraValue>& args) {
    ensureAtLeast(args, 2, "bhaago");
    std::vector<double> values = collectNumbers(args, "bhaago");
    if (values.size() < 2) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, "bhaago requires at least 2 numbers", 0, 0);
        throw std::runtime_error("bhaago");
    }
    double result = values[0];
    for (size_t i = 1; i < values.size(); ++i) {
        result /= values[i];
    }
    return MantraValue::number(result);
}

MantraValue builtinRemainder(const std::vector<MantraValue>& args) {
    ensureAtLeast(args, 2, "baki");
    std::vector<double> values = collectNumbers(args, "baki");
    if (values.size() < 2) {
        ErrorHandler::printError(ErrorType::RUNTIME_ERROR, "baki requires at least 2 numbers", 0, 0);
        throw std::runtime_error("baki");
    }
    double result = values[0];
    for (size_t i = 1; i < values.size(); ++i) {
        result = std::fmod(result, values[i]);
    }
    return MantraValue::number(result);
}

MantraValue builtinMean(const std::vector<MantraValue>& args) {
    std::vector<double> values = requireSeries(args, "madhyaman");
    return MantraValue::number(meanOf(values));
}

MantraValue builtinMedian(const std::vector<MantraValue>& args) {
    std::vector<double> values = requireSeries(args, "madhyika");
    return MantraValue::number(medianOf(std::move(values)));
}

MantraValue builtinMode(const std::vector<MantraValue>& args) {
    std::vector<double> values = requireSeries(args, "bahulak");
    return MantraValue::number(modeOf(values));
}

MantraValue builtinRange(const std::vector<MantraValue>& args) {
    std::vector<double> values = requireSeries(args, "vistar");
    return MantraValue::number(maxOf(values) - minOf(values));
}

MantraValue builtinStdDeviation(const std::vector<MantraValue>& args) {
    std::vector<double> values = requireSeries(args, "pramaan");
    return MantraValue::number(populationStdDev(values));
}

MantraValue builtinSum(const std::vector<MantraValue>& args) {
    std::vector<double> values = collectNumbers(args, "yogfal");
    return MantraValue::number(sumValues(values));
}

MantraValue builtinMaximum(const std::vector<MantraValue>& args) {
    std::vector<double> values = requireSeries(args, "adhiktam");
    return MantraValue::number(maxOf(values));
}

MantraValue builtinMinimum(const std::vector<MantraValue>& args) {
    std::vector<double> values = requireSeries(args, "nyuntam");
    return MantraValue::number(minOf(values));
}

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
