#include "repl.h"
#include "../error/error.h"
#include <algorithm>
#include <iostream>
#include <sstream>

namespace mantra {

namespace {

std::string toLowerCopy(const std::string& value) {
    std::string out;
    out.reserve(value.size());
    for (char ch : value) {
        out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
    }
    return out;
}

std::string trimCopy(const std::string& value) {
    const std::string whitespace = " \t\n\r";
    const auto start = value.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    const auto end = value.find_last_not_of(whitespace);
    return value.substr(start, end - start + 1);
}

std::string normalizeCommand(const std::string& input) {
    return toLowerCopy(trimCopy(input));
}

void printSeparator() {
    std::cout << "----------------------------------------" << std::endl;
}

void printHelpHeader() {
    std::cout << "MANTRA REPL सहायता (Help)" << std::endl;
    printSeparator();
}

void printHelpCommands() {
    std::cout << "Commands:" << std::endl;
    std::cout << "  band_karo / quit / exit  : REPL बंद करें" << std::endl;
    std::cout << "  saaf / clear             : स्क्रीन साफ करें" << std::endl;
    std::cout << "  madad / help             : यह सहायता दिखाएँ" << std::endl;
    std::cout << "  ?                        : shortcut for help" << std::endl;
    std::cout << std::endl;
}

void printHelpExamples() {
    std::cout << "Examples:" << std::endl;
    std::cout << "  dikhao = \"Namaste\"" << std::endl;
    std::cout << "  rakho x = 5 + 3" << std::endl;
    std::cout << "  dikhao = x" << std::endl;
    std::cout << "  agar x > 5 tab" << std::endl;
    std::cout << "    dikhao = \"bada hai\"" << std::endl;
    std::cout << "  warna" << std::endl;
    std::cout << "    dikhao = \"chota hai\"" << std::endl;
    std::cout << "  baarbaar i = 0 se 3 tak" << std::endl;
    std::cout << "    dikhao = i" << std::endl;
    std::cout << std::endl;
}

void printHelpKeywords() {
    std::cout << "Supported Keywords (Hindi/English mix):" << std::endl;
    std::cout << "  agar/if ... tab/then ... warna/else" << std::endl;
    std::cout << "  jodi ... tab ... nahole" << std::endl;
    std::cout << "  agre ... tab ... illandre" << std::endl;
    std::cout << "  endral ... illena" << std::endl;
    std::cout << "  baarbaar/while ..." << std::endl;
    std::cout << "  for i = 0 to 10 step 2" << std::endl;
    std::cout << "  kaam/function name(...) ... wapas/return ..." << std::endl;
    std::cout << "  or/ya, and/aur, not/nahi" << std::endl;
    std::cout << "  true/sach, false/jhooth" << std::endl;
    std::cout << std::endl;
}

void printHelpOperators() {
    std::cout << "Operators (precedence order):" << std::endl;
    std::cout << "  1. or/ya" << std::endl;
    std::cout << "  2. and/aur" << std::endl;
    std::cout << "  3. not/nahi" << std::endl;
    std::cout << "  4. == != < > <= >=" << std::endl;
    std::cout << "  5. + -" << std::endl;
    std::cout << "  6. * / %" << std::endl;
    std::cout << "  7. unary - not" << std::endl;
    std::cout << std::endl;
}

void printHelpStdlib() {
    std::cout << "Stdlib functions:" << std::endl;
    std::cout << "  print/println और सभी भाषा variants" << std::endl;
    std::cout << "  lo/input/loo" << std::endl;
    std::cout << "  lambai/length" << std::endl;
    std::cout << "  jodo_shabd/concat" << std::endl;
    std::cout << "  sankhya/toNumber" << std::endl;
    std::cout << "  shabd/toString" << std::endl;
    std::cout << "  type" << std::endl;
    std::cout << std::endl;
}

void printHelpLanguages() {
    std::cout << "Print keywords across 22 mixes:" << std::endl;
    std::cout << "  dikhao, kaado, dakho, dekhao, batavo" << std::endl;
    std::cout << "  dakhav, chupinchu, toro, kaaniku, jadi" << std::endl;
    std::cout << "  nohole, dekhaow, dakhoi, dekhau, darshaya" << std::endl;
    std::cout << "  waatav, dikhay, wekho, nungsi, nangi, dado" << std::endl;
    std::cout << std::endl;
}

void printHelpBlocks() {
    std::cout << "Blocks and indentation:" << std::endl;
    std::cout << "  if/while/for/function के बाद block लिखें" << std::endl;
    std::cout << "  block indent से शुरू होता है" << std::endl;
    std::cout << "  same indent पर back आने से block समाप्त होता है" << std::endl;
    std::cout << "  braces {} भी supported हैं" << std::endl;
    std::cout << "  example:" << std::endl;
    std::cout << "    agar x > 5 tab" << std::endl;
    std::cout << "      dikhao = x" << std::endl;
    std::cout << "    warna" << std::endl;
    std::cout << "      dikhao = 0" << std::endl;
    std::cout << std::endl;
}

void printHelpValues() {
    std::cout << "Values:" << std::endl;
    std::cout << "  number  : 10, 3.14" << std::endl;
    std::cout << "  string  : \"Namaste\"" << std::endl;
    std::cout << "  boolean : sach/true, jhooth/false" << std::endl;
    std::cout << "  null    : null" << std::endl;
    std::cout << "  array   : [1, 2, 3]" << std::endl;
    std::cout << std::endl;
}

void printHelpTips() {
    std::cout << "Tips:" << std::endl;
    std::cout << "  - REPL एक लाइन input लेता है" << std::endl;
    std::cout << "  - complex blocks के लिए script file लिखें" << std::endl;
    std::cout << "  - examples/ folder देखें" << std::endl;
    std::cout << "  - output गलत आए तो whitespace check करें" << std::endl;
    std::cout << "  - strings में Unicode allowed है" << std::endl;
    std::cout << std::endl;
}

void printHelpNotes() {
    std::cout << "Notes:" << std::endl;
    std::cout << "  - हर लाइन स्वतंत्र रूप से पार्स होती है" << std::endl;
    std::cout << "  - Errors आने पर REPL बंद नहीं होगा" << std::endl;
    std::cout << "  - String में Unicode समर्थित है" << std::endl;
    std::cout << "  - Numbers double precision में हैं" << std::endl;
    std::cout << std::endl;
}

} // namespace

Repl::Repl() = default;

void Repl::run() {
    printBanner();

    while (true) {
        std::cout << prompt();
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << std::endl;
            printGoodbye();
            break;
        }

        std::string trimmed = trim(line);
        if (trimmed.empty()) {
            continue;
        }

        if (isExitCommand(trimmed)) {
            printGoodbye();
            break;
        }

        if (isClearCommand(trimmed)) {
            clearScreen();
            continue;
        }

        if (isHelpCommand(trimmed)) {
            printHelp();
            continue;
        }

        std::string source = line;
        source.push_back('\n');

        try {
            Lexer lexer(source);
            auto tokens = lexer.tokenize();
            Parser parser(tokens);
            auto program = parser.parseProgram();

            if (parser.hasError()) {
                printErrorHint();
                continue;
            }

            if (!program || program->statements.empty()) {
                continue;
            }

            if (program->statements.size() == 1 &&
                program->statements.front()->type == NodeType::EXPR_STMT) {
                auto value = interpreter.evaluate(*program->statements.front());
                if (value.type != ValueType::Null) {
                    std::cout << value.toString() << std::endl;
                }
            } else {
                interpreter.interpret(*program);
            }
        } catch (const std::exception&) {
            printErrorHint();
        }
    }
}

void Repl::printBanner() const {
    std::cout << "MANTRA v0.1.0 India Ki Programming Language" << std::endl;
    std::cout << "Type 'madad' or 'help' for सहायता." << std::endl;
    std::cout << "Type 'band_karo', 'quit', or 'exit' to exit." << std::endl;
    std::cout << "Prompt: mantra>" << std::endl;
    std::cout << std::endl;
}

void Repl::printGoodbye() const {
    std::cout << "Dhanyavaad! Phir milenge." << std::endl;
}

void Repl::printHelp() const {
    printHelpHeader();
    printHelpCommands();
    printHelpExamples();
    printHelpKeywords();
    printHelpOperators();
    printHelpStdlib();
    printHelpLanguages();
    printHelpBlocks();
    printHelpValues();
    printHelpTips();
    printHelpNotes();
}

void Repl::printErrorHint() const {
    std::cout << "Input में त्रुटि है. कृपया पुनः प्रयास करें." << std::endl;
}

std::string Repl::prompt() const {
    return "mantra> ";
}

std::string Repl::trim(const std::string& value) const {
    return trimCopy(value);
}

bool Repl::isExitCommand(const std::string& input) const {
    std::string lower = normalizeCommand(input);
    return lower == "exit" || lower == "quit" || lower == "band_karo" || lower == "band karo";
}

bool Repl::isClearCommand(const std::string& input) const {
    std::string lower = normalizeCommand(input);
    return lower == "saaf" || lower == "clear";
}

bool Repl::isHelpCommand(const std::string& input) const {
    std::string lower = normalizeCommand(input);
    return lower == "madad" || lower == "help" || lower == "?";
}

void Repl::clearScreen() const {
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}





















} // namespace mantra
