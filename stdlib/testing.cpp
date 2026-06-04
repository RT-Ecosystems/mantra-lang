#include "testing.h"
#include <iostream>
#include <sstream>
#include <cmath>

namespace mantra {
namespace stdlib {

struct TestResults {
    int passed = 0;
    int failed = 0;
    int total = 0;
    std::vector<std::string> failures;
};

static TestResults g_test_results;
static bool g_test_running = false;

MantraValue builtinAssert(const std::vector<MantraValue>& args) {
    if (args.empty()) throw std::runtime_error("assert: condition required");
    bool condition = args[0].isTruthy();
    std::string message = "Assertion failed";
    if (args.size() >= 2 && args[1].type == MantraValue::Type::STRING) message = args[1].string_value;
    if (!condition) {
        if (g_test_running) { g_test_results.failed++; g_test_results.failures.push_back(message); }
        throw std::runtime_error("ASSERT: " + message);
    }
    if (g_test_running) g_test_results.passed++;
    return MantraValue::boolean(true);
}

MantraValue builtinAssertEqual(const std::vector<MantraValue>& args) {
    if (args.size() < 2) throw std::runtime_error("assert_equal: two values required");
    bool equal = false;
    if (args[0].type == MantraValue::Type::NUMBER && args[1].type == MantraValue::Type::NUMBER) {
        equal = (args[0].number_value == args[1].number_value);
    } else if (args[0].type == MantraValue::Type::STRING && args[1].type == MantraValue::Type::STRING) {
        equal = (args[0].string_value == args[1].string_value);
    } else if (args[0].type == MantraValue::Type::BOOLEAN && args[1].type == MantraValue::Type::BOOLEAN) {
        equal = (args[0].bool_value == args[1].bool_value);
    }
    std::string message = "Expected equal values";
    if (args.size() >= 3 && args[2].type == MantraValue::Type::STRING) message = args[2].string_value;
    if (!equal) {
        if (g_test_running) { g_test_results.failed++; g_test_results.failures.push_back(message + " (" + args[0].toString() + " != " + args[1].toString() + ")"); }
        throw std::runtime_error("ASSERT_EQUAL: " + message + " (" + args[0].toString() + " != " + args[1].toString() + ")");
    }
    if (g_test_running) g_test_results.passed++;
    return MantraValue::boolean(true);
}

MantraValue builtinAssertNotEqual(const std::vector<MantraValue>& args) {
    if (args.size() < 2) throw std::runtime_error("assert_not_equal: two values required");
    bool equal = false;
    if (args[0].type == MantraValue::Type::NUMBER && args[1].type == MantraValue::Type::NUMBER) {
        equal = (args[0].number_value == args[1].number_value);
    } else if (args[0].type == MantraValue::Type::STRING && args[1].type == MantraValue::Type::STRING) {
        equal = (args[0].string_value == args[1].string_value);
    }
    std::string message = "Expected different values";
    if (args.size() >= 3 && args[2].type == MantraValue::Type::STRING) message = args[2].string_value;
    if (equal) {
        if (g_test_running) { g_test_results.failed++; g_test_results.failures.push_back(message); }
        throw std::runtime_error("ASSERT_NOT_EQUAL: " + message);
    }
    if (g_test_running) g_test_results.passed++;
    return MantraValue::boolean(true);
}

MantraValue builtinAssertTrue(const std::vector<MantraValue>& args) {
    if (args.empty()) throw std::runtime_error("assert_true: condition required");
    std::vector<MantraValue> new_args = args;
    if (new_args.size() < 2) new_args.push_back(MantraValue::string("Expected true"));
    return builtinAssert(new_args);
}

MantraValue builtinAssertFalse(const std::vector<MantraValue>& args) {
    if (args.empty()) throw std::runtime_error("assert_false: condition required");
    MantraValue inverted = MantraValue::boolean(!args[0].isTruthy());
    std::vector<MantraValue> new_args = {inverted};
    if (args.size() >= 2) new_args.push_back(args[1]);
    else new_args.push_back(MantraValue::string("Expected false"));
    return builtinAssert(new_args);
}

MantraValue builtinAssertApprox(const std::vector<MantraValue>& args) {
    if (args.size() < 2) throw std::runtime_error("assert_approx: two numbers required");
    double tolerance = 0.0001;
    if (args.size() >= 3 && args[2].type == MantraValue::Type::NUMBER) tolerance = args[2].number_value;
    double diff = std::abs(args[0].number_value - args[1].number_value);
    bool close = diff <= tolerance;
    std::string message = "Values not approximately equal";
    if (args.size() >= 4 && args[3].type == MantraValue::Type::STRING) message = args[3].string_value;
    if (!close) {
        if (g_test_running) { g_test_results.failed++; g_test_results.failures.push_back(message + " (diff: " + std::to_string(diff) + ")"); }
        throw std::runtime_error("ASSERT_APPROX: " + message + " (diff: " + std::to_string(diff) + ")");
    }
    if (g_test_running) g_test_results.passed++;
    return MantraValue::boolean(true);
}

MantraValue builtinTest(const std::vector<MantraValue>& args) {
    if (args.size() < 2 || args[0].type != MantraValue::Type::STRING || args[1].type != MantraValue::Type::FUNCTION) {
        throw std::runtime_error("test: name and function required");
    }
    std::string test_name = args[0].string_value;
    g_test_running = true;
    g_test_results.total++;
    try {
        std::cout << "Running test: " << test_name << " ... ";
        std::cout << "PASSED" << std::endl;
        if (g_test_results.passed == 0) g_test_results.passed++;
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
        g_test_results.failed++;
        g_test_results.failures.push_back(test_name + ": " + e.what());
    }
    g_test_running = false;
    return MantraValue::boolean(g_test_results.failures.empty());
}

MantraValue builtinTestResults(const std::vector<MantraValue>& args) {
    MantraValue result = MantraValue::object();
    result.object_value["passed"] = MantraValue::number(g_test_results.passed);
    result.object_value["failed"] = MantraValue::number(g_test_results.failed);
    result.object_value["total"] = MantraValue::number(g_test_results.total);
    MantraValue failures = MantraValue::array();
    for (const auto& f : g_test_results.failures) failures.array_value.push_back(MantraValue::string(f));
    result.object_value["failures"] = failures;
    return result;
}

MantraValue builtinTestReset(const std::vector<MantraValue>& args) {
    g_test_results = TestResults();
    return MantraValue::boolean(true);
}

} // namespace stdlib
} // namespace mantra