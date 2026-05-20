#include "web/mtr_parser.h"
#include "web/html_generator.h"
#include "core/interpreter.h"
#include "core/lexer.h"
#include "core/parser.h"
#include "core/repl.h"
#include "semantic/semantic_analyzer.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

bool runSource(const std::string& source) {
    mantra::Lexer lexer(source);
    auto tokens = lexer.tokenize();

    mantra::Parser parser(tokens);
    auto program = parser.parseProgram();
    if (parser.hasError() || !program) {
        return false;
    }

    mantra::SemanticAnalyzer semantic;
    if (!semantic.analyze(*program)) {
        return false;
    }

    mantra::Interpreter interpreter;
    interpreter.interpret(*program);
    return true;
}

bool runFile(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        std::cerr << "Unable to open file: " << path << std::endl;
        return false;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return runSource(buffer.str());
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc > 1) {
        return runFile(argv[1]) ? 0 : 1;
    }

    mantra::Repl repl;
    repl.run();
    return 0;
}
