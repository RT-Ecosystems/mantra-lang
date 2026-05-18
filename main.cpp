#include "core/interpreter.h"
#include "core/lexer.h"
#include "core/parser.h"
#include "core/repl.h"
#include "compiler/compiler.h"
#include "semantic/semantic_analyzer.h"
#include "vm/vm.h"
#include "error/error.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr int kExitOk = 0;
constexpr int kExitUsage = 64;
constexpr int kExitNoInput = 66;
constexpr int kExitRuntime = 70;

void printUsage(const std::string& program) {
    std::cout << "MANTRA Language Interpreter\n";
    std::cout << "उपयोग: " << program << " [options] [file.mtr]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help      यह सहायता संदेश दिखाएं\n";
    std::cout << "  --version   संस्करण दिखाएं\n";
    std::cout << "  --lex       केवल टोकन प्रिंट करें\n";
    std::cout << "\nउदाहरण:\n";
    std::cout << "  " << program << " hello.mtr\n";
    std::cout << "  " << program << " --lex hello.mtr\n";
    std::cout << "  " << program << " --version\n";
    std::cout << "\nटिप: बिना फाइल दिए REPL शुरू होगा।\n";
    std::cout << "उदाहरण REPL: dikhao \"Namaste Duniya\"\n";
    std::cout << "REPL से बाहर निकलने के लिए: exit या quit लिखें\n";
    std::cout << "REPL में हिंदी/अंग्रेज़ी मिश्रित कमांड समर्थित हैं\n";
}

void printVersion() {
    std::cout << "MANTRA संस्करण 0.1.0" << std::endl;
}

bool hasMantraExtension(const std::string& path) {
    const std::string suffix1 = ".mantra";
    const std::string suffix2 = ".mtr";
    if (path.size() >= suffix1.size() &&
        path.compare(path.size() - suffix1.size(), suffix1.size(), suffix1) == 0) {
        return true;
    }
    if (path.size() >= suffix2.size() &&
        path.compare(path.size() - suffix2.size(), suffix2.size(), suffix2) == 0) {
        return true;
    }
    return false;
}

bool readFileContents(const std::string& path, std::string& out_source) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    out_source = buffer.str();
    return true;
}

} // namespace

int main(int argc, char** argv) {
    std::vector<std::string> args(argv + 1, argv + argc);
    bool lex_only = false;
    bool show_help = false;
    bool show_version = false;
    std::string file_path;

    for (const auto& arg : args) {
        if (arg == "--help") {
            show_help = true;
        } else if (arg == "--version") {
            show_version = true;
        } else if (arg == "--lex") {
            lex_only = true;
        } else if (!arg.empty() && arg[0] == '-') {
            std::cerr << "अज्ञात विकल्प: " << arg << std::endl;
            printUsage(argv[0]);
            return kExitUsage;
        } else if (file_path.empty()) {
            file_path = arg;
        } else {
            std::cerr << "एक से अधिक फाइल नहीं दी जा सकती" << std::endl;
            return kExitUsage;
        }
    }

    if (show_help) {
        printUsage(argv[0]);
        return kExitOk;
    }

    if (show_version) {
        printVersion();
        return kExitOk;
    }

    if (file_path.empty()) {
        mantra::Repl repl;
        repl.run();
        return kExitOk;
    }

    if (!hasMantraExtension(file_path)) {
        std::cerr << "केवल .mantra या .mtr फाइल स्वीकार है" << std::endl;
        return kExitUsage;
    }

    std::string source;
    if (!readFileContents(file_path, source)) {
        mantra::ErrorHandler::printError(mantra::ErrorType::FILE_NOT_FOUND,
                                         "फाइल नहीं मिली: " + file_path,
                                         0, 0);
        return kExitNoInput;
    }

    if (source.empty()) {
        std::cerr << "खाली फाइल" << std::endl;
        return kExitNoInput;
    }

    mantra::Lexer lexer(source);
    auto tokens = lexer.tokenize();
    if (lex_only) {
        lexer.printTokens();
        return kExitOk;
    }

    mantra::Parser parser(tokens);
    auto program = parser.parseProgram();

    if (parser.hasError()) {
        return kExitRuntime;
    }

    if (!program) {
        std::cerr << "प्रोग्राम नहीं पढ़ा जा सका" << std::endl;
        return kExitRuntime;
    }

    mantra::SemanticAnalyzer semantic;
    if (!semantic.analyze(*program)) {
        return kExitRuntime;
    }

    try {
        mantra::BytecodeCompiler compiler;
        auto chunk = compiler.compile(*program);
        mantra::VM vm;
        if (vm.execute(chunk)) {
            return kExitOk;
        }
    } catch (const std::exception&) {
        // Fall back to the tree-walk interpreter.
    }

    try {
        mantra::Interpreter interpreter;
        interpreter.interpret(*program);
    } catch (const std::exception&) {
        return kExitRuntime;
    }

    return kExitOk;
}
