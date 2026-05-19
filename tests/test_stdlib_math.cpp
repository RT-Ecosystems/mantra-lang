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
    interpreter.interpret(*program);
    return output.str();
}

void test_math_aliases() {
    const std::string output = runProgram(
        "print jodo(1, 2, 3)\n"
        "print varga(16)\n"
        "print karan(16)\n"
        "print mul(16)\n"
        "print madhyaman([1, 2, 3, 4, 5])\n"
        "print median([1, 2, 3, 4])\n"
        "print mode([1, 1, 2, 3])\n"
        "print range([1, 4, 9])\n"
        "print sum([1, 2, 3, 4, 5])\n"
        "print max([1, 9, 3])\n"
        "print min([1, 9, 3])\n"
        "print PI\n"
        "print E\n");

    require(output == "6\n4\n4\n4\n3\n2.5\n1\n8\n15\n9\n1\n3.14159265358979\n2.71828182845905\n",
            "Unexpected math output");
}

} // namespace

void runMathTests() {
    test_math_aliases();
    std::cout << "Math stdlib tests passed" << std::endl;
}
