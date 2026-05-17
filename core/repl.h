#ifndef MANTRA_REPL_H
#define MANTRA_REPL_H

#include "interpreter.h"
#include "parser.h"
#include "lexer.h"
#include <string>

namespace mantra {

class Repl {
public:
    Repl();
    void run();

private:
    Interpreter interpreter;

    void printWelcome() const;
    void printGoodbye() const;
    void printHint() const;
    std::string prompt(bool first_line) const;
    bool isExitCommand(const std::string& input) const;
    bool isInputComplete(const std::string& input) const;
    std::string trim(const std::string& value) const;
    void updateDelimiterBalance(char ch, int& braces, int& parens) const;
    bool isBlank(const std::string& value) const;
    std::string normalizeLine(const std::string& line) const;
};

} // namespace mantra

#endif // MANTRA_REPL_H
