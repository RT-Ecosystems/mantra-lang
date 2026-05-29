#include "core/interpreter.h"
#include "core/lexer.h"
#include "core/parser.h"
#include "test_runner.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << std::endl;
        std::abort();
    }
}

struct CoutRedirect {
    explicit CoutRedirect(std::ostream& stream)
        : original(std::cout.rdbuf(stream.rdbuf())) {}

    ~CoutRedirect() {
        std::cout.rdbuf(original);
    }

    std::streambuf* original;
};

std::string runProgram(const std::string& source) {
    mantra::Lexer lexer(source);
    auto tokens = lexer.tokenize();
    mantra::Parser parser(tokens);
    auto program = parser.parseProgram();
    require(program != nullptr, "Program node missing");
    require(!parser.hasError(), "Unexpected parser error");

    mantra::Interpreter interpreter;
    std::ostringstream output;
    CoutRedirect redirect(output);
    try {
        interpreter.interpret(*program);
    } catch (const std::exception& e) {
        std::string error_msg = std::string("Interpretation failed: ") + e.what();
        require(false, error_msg.c_str());
    }
    return output.str();
}

void test_math_aliases() {
    struct Case {
        const char* name;
        const char* source;
        const char* expected;
    };

    const Case cases[] = {
        {"add", "print jodo(1, 2, 3)\n", "6\n"},
        {"subtract", "print ghato(10, 3, 2)\n", "5\n"},
        {"multiply", "print guno(2, 3, 4)\n", "24\n"},
        {"divide", "print bhaago(100, 5, 2)\n", "10\n"},
        {"remainder", "print baki(20, 6)\n", "2\n"},
        {"sqrt-varga", "print varga(16)\n", "4\n"},
        {"sqrt-karan", "print karan(16)\n", "4\n"},
        {"sqrt-mul", "print mul(16)\n", "4\n"},
        {"mean", "print madhyaman([1, 2, 3, 4, 5])\n", "3\n"},
        {"median", "print median([1, 2, 3, 4])\n", "2.5\n"},
        {"mode", "print mode([1, 1, 2, 3])\n", "1\n"},
        {"range", "print range([1, 4, 9])\n", "8\n"},
        {"std-dev", "print pramaan([2, 4, 4, 4, 5, 5, 7, 9])\n", "2\n"},
        {"sum", "print sum([1, 2, 3, 4, 5])\n", "15\n"},
        {"max", "print max([1, 9, 3])\n", "9\n"},
        {"min", "print min([1, 9, 3])\n", "1\n"},
        {"pi-constant", "print PI\n", "3.14159265358979\n"},
        {"e-constant", "print E\n", "2.71828182845905\n"},
    };

    for (const auto& test_case : cases) {
        try {
            auto result = runProgram(test_case.source);
            require(result == test_case.expected, test_case.name);
        } catch (...) {
            require(false, test_case.name);
        }
    }
}

} // namespace

void runMathTests() {
    test_math_aliases();
    std::cout << "Math stdlib tests passed" << std::endl;
}
