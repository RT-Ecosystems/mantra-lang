#include "core/lexer.h"
#include "core/parser.h"
#include "semantic/semantic_analyzer.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

using mantra::Lexer;
using mantra::Parser;
using mantra::SemanticAnalyzer;

namespace {

void require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "टेस्ट विफल: " << message << std::endl;
        std::abort();
    }
}

std::unique_ptr<mantra::ProgramNode> parseProgram(const std::string& source, bool* parser_had_error = nullptr) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parseProgram();
    if (parser_had_error) {
        *parser_had_error = parser.hasError();
    }
    return program;
}

bool analyze(const std::string& source) {
    bool parser_error = false;
    auto program = parseProgram(source, &parser_error);
    require(program != nullptr, "Program node missing");
    require(!parser_error, "Unexpected parser error");
    SemanticAnalyzer analyzer;
    return analyzer.analyze(*program);
}

void testValidProgram() {
    require(analyze("function add(a, b) { return a + b }\nlet x = add(1, 2)\nprint x\n"),
            "Valid program should pass semantic analysis");
}

void testUndefinedVariable() {
    require(!analyze("print missing_value\n"), "Undefined variable should fail");
}

void testDuplicateDeclaration() {
    require(!analyze("let x = 1\nlet x = 2\n"), "Duplicate declaration should fail");
}

void testWrongArgumentCount() {
    require(!analyze("function add(a, b) { return a + b }\nadd(1)\n"), "Wrong function arity should fail");
}

void testControlFlowOutsideContext() {
    require(!analyze("return 1\n"), "return outside function should fail");
    require(!analyze("break\n"), "break outside loop should fail");
    require(!analyze("continue\n"), "continue outside loop should fail");
}

} // namespace

int main() {
    testValidProgram();
    testUndefinedVariable();
    testDuplicateDeclaration();
    testWrongArgumentCount();
    testControlFlowOutsideContext();
    std::cout << "सभी semantic टेस्ट सफल रहे।" << std::endl;
    return 0;
}
