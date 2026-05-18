#include "compiler/compiler.h"
#include "core/lexer.h"
#include "core/parser.h"
#include "semantic/semantic_analyzer.h"
#include "vm/vm.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace {

void require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "टेस्ट विफल: " << message << std::endl;
        std::abort();
    }
}

std::string runProgram(const std::string& source) {
    mantra::Lexer lexer(source);
    auto tokens = lexer.tokenize();
    mantra::Parser parser(tokens);
    auto program = parser.parseProgram();
    require(!parser.hasError(), "Parser error");
    require(program != nullptr, "Program missing");

    mantra::SemanticAnalyzer analyzer;
    require(analyzer.analyze(*program), "Semantic analysis failed");

    mantra::BytecodeCompiler compiler;
    auto chunk = compiler.compile(*program);
    mantra::VM vm;

    std::ostringstream output;
    auto* previous = std::cout.rdbuf(output.rdbuf());
    bool ok = vm.execute(chunk);
    std::cout.rdbuf(previous);

    require(ok, "VM execution failed");
    return output.str();
}

void testCalculator() {
    const std::string output = runProgram(
        "rakho a = 10\n"
        "rakho b = 5\n"
        "dikhao = a + b\n"
        "dikhao = a - b\n"
        "dikhao = a * b\n"
        "dikhao = a / b\n");
    require(output == "15\n5\n50\n2\n", "Calculator output mismatch");
}

void testLoop() {
    const std::string output = runProgram(
        "baarbaar i = 1 se 3 tak\n"
        "  dikhao = i\n");
    require(output == "1\n2\n3\n", "Loop output mismatch");
}

void testFunctionCall() {
    const std::string output = runProgram(
        "kaam jodoba(a, b)\n"
        "  wapas a + b\n"
        "result = jodoba(10, 20)\n"
        "dikhao = result\n");
    require(output == "30\n", "Function output mismatch");
}

void testConditionAndArrayAccess() {
    const std::string output = runProgram(
        "rakho values = [1, 2, 3]\n"
        "agar values[1] == 2 tab\n"
        "  dikhao = \"ok\"\n"
        "warna\n"
        "  dikhao = \"no\"\n");
    require(output == "ok\n", "Condition/array output mismatch");
}

} // namespace

int main() {
    testCalculator();
    testLoop();
    testFunctionCall();
    testConditionAndArrayAccess();
    std::cout << "सभी VM टेस्ट सफल रहे।" << std::endl;
    return 0;
}
