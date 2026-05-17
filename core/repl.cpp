#include "repl.h"
#include "error.h"
#include <iostream>
#include <sstream>

namespace mantra {

Repl::Repl() = default;

void Repl::run() {
    std::string buffer;
    bool first_line = true;
    printWelcome();

    while (true) {
        std::cout << prompt(first_line);

        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << std::endl;
            printGoodbye();
            break;
        }
        line = normalizeLine(line);

        std::string trimmed = trim(line);
        if (buffer.empty() && isBlank(trimmed)) {
            continue;
        }
        if (buffer.empty() && isExitCommand(trimmed)) {
            printGoodbye();
            break;
        }

        buffer += line;
        buffer += "\n";

        if (!isInputComplete(buffer)) {
            first_line = false;
            continue;
        }

        Lexer lexer(buffer);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto program = parser.parseProgram();

        if (!parser.hasError()) {
            if (program && program->statements.size() == 1 &&
                program->statements.front()->type == NodeType::ExprStatement) {
                auto value = interpreter.evaluate(*program->statements.front());
                if (value.type != ValueType::Null) {
                    std::cout << value.toString() << std::endl;
                }
            } else if (program) {
                interpreter.interpret(*program);
            }
        } else {
            std::cerr << "इनपुट में त्रुटि, कृपया पुनः प्रयास करें।" << std::endl;
            printHint();
        }

        buffer.clear();
        first_line = true;
    }
}

void Repl::printWelcome() const {
    std::cout << "MANTRA REPL में आपका स्वागत है।" << std::endl;
    std::cout << "बाहर निकलने के लिए exit/quit या band karo लिखें।" << std::endl;
    std::cout << "मल्टी-लाइन के लिए {} या () बंद होने तक लिख सकते हैं।" << std::endl;
    std::cout << "उदाहरण: dikhao \"Namaste Duniya\"" << std::endl;
    std::cout << "उदाहरण: baarbaar i = 1 se 3 tak { dikhao i }" << std::endl;
}

void Repl::printGoodbye() const {
    std::cout << "धन्यवाद! फिर मिलेंगे।" << std::endl;
}

void Repl::printHint() const {
    std::cout << "संकेत: स्टेटमेंट को नई लाइन या ';' से अलग करें।" << std::endl;
    std::cout << "संकेत: ब्लॉक के लिए { और } का उपयोग करें।" << std::endl;
}

std::string Repl::prompt(bool first_line) const {
    if (first_line) {
        return "mantra> ";
    }
    return "....> ";
}

bool Repl::isExitCommand(const std::string& input) const {
    return input == "exit" || input == "quit" || input == "band karo";
}

bool Repl::isInputComplete(const std::string& input) const {
    int braces = 0;
    int parens = 0;
    bool in_single = false;
    bool in_double = false;

    for (size_t i = 0; i < input.size(); ++i) {
        char ch = input[i];
        if (ch == '\\') {
            ++i;
            continue;
        }

        if (!in_double && ch == '\'') {
            in_single = !in_single;
            continue;
        }

        if (!in_single && ch == '"') {
            in_double = !in_double;
            continue;
        }

        if (in_single || in_double) {
            continue;
        }

        updateDelimiterBalance(ch, braces, parens);
    }

    return braces <= 0 && parens <= 0 && !in_single && !in_double;
}

std::string Repl::trim(const std::string& value) const {
    const std::string whitespace = " \t\n\r";
    const auto start = value.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    const auto end = value.find_last_not_of(whitespace);
    return value.substr(start, end - start + 1);
}

void Repl::updateDelimiterBalance(char ch, int& braces, int& parens) const {
    if (ch == '{') {
        braces++;
    } else if (ch == '}') {
        braces--;
    } else if (ch == '(') {
        parens++;
    } else if (ch == ')') {
        parens--;
    }
}

bool Repl::isBlank(const std::string& value) const {
    for (char ch : value) {
        if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r') {
            return false;
        }
    }
    return true;
}

std::string Repl::normalizeLine(const std::string& line) const {
    if (!line.empty() && line.back() == '\r') {
        return line.substr(0, line.size() - 1);
    }
    return line;
}

} // namespace mantra
