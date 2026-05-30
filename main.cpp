#include "web/mtr_parser.h"
#include "web/html_generator.h"
#include "core/interpreter.h"
#include "core/lexer.h"
#include "core/parser.h"
#include "core/repl.h"
#include "semantic/semantic_analyzer.h"
#include "compiler/compiler.h"
#include "mantra/vm/vm.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

bool runSource(const std::string& source, bool use_vm = false) {
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

    if (use_vm) {
        mantra::BytecodeCompiler compiler;
        auto bytecode = compiler.compile(*program);
        
        mantra::VM vm;
        if (!vm.execute(*bytecode, std::cout)) {
            std::cerr << "VM Error: " << vm.lastError() << std::endl;
            return false;
        }
    } else {
        mantra::Interpreter interpreter;
        interpreter.interpret(*program);
    }
    
    return true;
}

bool runFile(const std::string& path, bool use_vm = false) {
    std::ifstream input(path);
    if (!input) {
        std::cerr << "Unable to open file: " << path << std::endl;
        return false;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return runSource(buffer.str(), use_vm);
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc > 1) {
        if (argc > 2 && std::string(argv[1]) == "--vm") {
            return runFile(argv[2], true) ? 0 : 1;
        }
        return runFile(argv[1]) ? 0 : 1;
    }

    mantra::Repl repl;
    repl.run();
    return 0;
}
