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

    void printBanner() const;
    void printGoodbye() const;
    void printHelp() const;
    void printErrorHint() const;

    std::string prompt() const;
    std::string trim(const std::string& value) const;

    bool isExitCommand(const std::string& input) const;
    bool isClearCommand(const std::string& input) const;
    bool isHelpCommand(const std::string& input) const;

    void clearScreen() const;
};

} // namespace mantra

#endif // MANTRA_REPL_H
