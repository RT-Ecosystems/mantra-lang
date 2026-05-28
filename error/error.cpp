#include "error.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace mantra {
namespace {

int error_count = 0;
int warning_count = 0;
int verbosity_level = 1;

std::string toLowerCopy(const std::string& value) {
    std::string out;
    out.reserve(value.size());
    for (char ch : value) {
        out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
    }
    return out;
}

const std::unordered_map<std::string, UserLanguage>& keywordLanguageMap() {
    static const std::unordered_map<std::string, UserLanguage> map = {
        {"dikhao", UserLanguage::Hinglish}, {"agar", UserLanguage::Hinglish},
        {"kaado", UserLanguage::Tanglish}, {"endral", UserLanguage::Tanglish},
        {"dakho", UserLanguage::Punglish}, {"jekr", UserLanguage::Punglish},
        {"dekhao", UserLanguage::Benglish}, {"jodi", UserLanguage::Benglish},
        {"batavo", UserLanguage::Gujlish}, {"jor", UserLanguage::Gujlish},
        {"dakhav", UserLanguage::Marathlish}, {"jar", UserLanguage::Marathlish},
        {"chupinchu", UserLanguage::Telglish}, {"ante", UserLanguage::Telglish},
        {"toro", UserLanguage::Kanglish}, {"agre", UserLanguage::Kanglish},
        {"kaaniku", UserLanguage::Manglish}, {"engil", UserLanguage::Manglish},
        {"jadi", UserLanguage::Odlish}, {"jabat", UserLanguage::Odlish},
        {"nohole", UserLanguage::Asslish}, {"jabotloi", UserLanguage::Asslish},
        {"dekhaow", UserLanguage::Maithlish}, {"je", UserLanguage::Maithlish},
        {"dakhoi", UserLanguage::Konklish}, {"zor", UserLanguage::Konklish},
        {"dekhau", UserLanguage::Neplish}, {"yedi", UserLanguage::Neplish},
        {"darshaya", UserLanguage::Sanglish}, {"yadi", UserLanguage::Sanglish},
        {"waatav", UserLanguage::Kashlish}, {"galaw", UserLanguage::Kashlish},
        {"dikhay", UserLanguage::Sindlish}, {"khoota", UserLanguage::Sindlish},
        {"wekho", UserLanguage::Doglish}, {"ger", UserLanguage::Doglish},
        {"nungsi", UserLanguage::Manlish}, {"amare", UserLanguage::Manlish},
        {"nangi", UserLanguage::Bodlish}, {"bodo", UserLanguage::Bodlish},
        {"dado", UserLanguage::Santlish}, {"ama", UserLanguage::Santlish},
        {"print", UserLanguage::English}, {"if", UserLanguage::English},
        {"function", UserLanguage::English}, {"return", UserLanguage::English}
    };
    return map;
}

std::string localizedPrefix(UserLanguage language, const std::string& english) {
    switch (language) {
        case UserLanguage::Hinglish: return "त्रुटि: " + english;
        case UserLanguage::Tanglish: return "Pizhai: " + english;
        case UserLanguage::Punglish: return "Galti: " + english;
        case UserLanguage::Benglish: return "Bhool: " + english;
        case UserLanguage::Gujlish: return "Bhool: " + english;
        case UserLanguage::Marathlish: return "Chook: " + english;
        case UserLanguage::Telglish: return "Tappu: " + english;
        case UserLanguage::Kanglish: return "Tappa: " + english;
        case UserLanguage::Manglish: return "Thett: " + english;
        case UserLanguage::Odlish: return "Bhul: " + english;
        case UserLanguage::Urlish: return "Ghalat: " + english;
        case UserLanguage::Asslish: return "Bhool: " + english;
        case UserLanguage::Maithlish: return "Bhul: " + english;
        case UserLanguage::Konklish: return "Chuk: " + english;
        case UserLanguage::Neplish: return "Galti: " + english;
        case UserLanguage::Sanglish: return "त्रुटिः: " + english;
        case UserLanguage::Kashlish: return "Khata: " + english;
        case UserLanguage::Sindlish: return "Galti: " + english;
        case UserLanguage::Doglish: return "Ghalti: " + english;
        case UserLanguage::Manlish: return "Thabuk: " + english;
        case UserLanguage::Bodlish: return "Khontai: " + english;
        case UserLanguage::Santlish: return "Gomke: " + english;
        case UserLanguage::English: return english;
        default: return english;
    }
}

const char* colorForTitle(const std::string& title) {
    if (title == "warning") {
        return "\033[33m";
    }
    return "\033[31m";
}

std::string errorMessageForCode(ErrorCode code) {
    switch (code) {
        case ErrorCode::MISSING_THEN: return "missing then/tab";
        case ErrorCode::VAR_UNDEFINED: return "undefined variable";
        case ErrorCode::TYPE_MISMATCH: return "type mismatch";
        case ErrorCode::SYNTAX_ERROR: return "syntax error";
        case ErrorCode::LET_MISSING_ID: return "missing identifier after let/rakho";
        case ErrorCode::INVALID_ASSIGN: return "invalid assignment";
        case ErrorCode::MISSING_RPAREN: return "missing ')'";
        case ErrorCode::MISSING_RBRACE: return "missing '}'";
        case ErrorCode::UNDEFINED_FUNCTION: return "undefined function";
        case ErrorCode::WRONG_ARG_COUNT: return "wrong argument count";
        case ErrorCode::UNKNOWN_IDENTIFIER: return "unknown identifier";
        case ErrorCode::INVALID_OPERATION: return "invalid operation";
        case ErrorCode::FILE_NOT_FOUND: return "file not found";
        case ErrorCode::IO_ERROR: return "I/O error";
        default: return "error";
    }
}

std::unordered_map<UserLanguage, std::string> buildMessages(const std::string& detail) {
    std::unordered_map<UserLanguage, std::string> out;
    for (UserLanguage lang : {
             UserLanguage::Hinglish, UserLanguage::Tanglish, UserLanguage::Punglish, UserLanguage::Benglish,
             UserLanguage::Gujlish, UserLanguage::Marathlish, UserLanguage::Telglish, UserLanguage::Kanglish,
             UserLanguage::Manglish, UserLanguage::Odlish, UserLanguage::Urlish, UserLanguage::Asslish,
             UserLanguage::Maithlish, UserLanguage::Konklish, UserLanguage::Neplish, UserLanguage::Sanglish,
             UserLanguage::Kashlish, UserLanguage::Sindlish, UserLanguage::Doglish, UserLanguage::Manlish,
             UserLanguage::Bodlish, UserLanguage::Santlish, UserLanguage::English}) {
        out.emplace(lang, localizedPrefix(lang, detail));
    }
    return out;
}

void printMantraError(const MantraError& err, const std::string& title) {
    if (verbosity_level == 0) {
        return;
    }
    if (title == "warning") {
        ++warning_count;
    } else {
        ++error_count;
    }

    std::ostringstream out;
    out << "\n" << colorForTitle(title) << "=== MANTRA " << title << " ===\033[0m\n";
    if (err.line > 0 && err.column > 0) {
        out << "Location: line " << err.line << ", col " << err.column << "\n";
    }

    auto it_en = err.messages.find(UserLanguage::English);
    if (it_en != err.messages.end()) {
        std::istringstream lines(it_en->second);
        std::string line;
        while (std::getline(lines, line)) {
            out << line << "\n";
        }
    } else {
        std::istringstream lines(errorMessageForCode(err.code) + ": " + err.detail);
        std::string line;
        while (std::getline(lines, line)) {
            out << line << "\n";
        }
    }

    auto it_hi = err.messages.find(UserLanguage::Hinglish);
    if (verbosity_level > 1 && it_hi != err.messages.end()) {
        out << it_hi->second << "\n";
    }

    std::cerr << out.str();
}

MantraError makeError(ErrorType type, ErrorCode code, const std::string& details, int line, int col) {
    MantraError err;
    err.type = type;
    err.code = code;
    err.detail = details;
    err.messages = buildMessages(errorMessageForCode(code) + ": " + details);
    err.line = line;
    err.column = col;
    return err;
}

} // namespace

UserLanguage detectUserLanguage(const std::vector<std::string>& lexemes) {
    std::unordered_map<UserLanguage, int> counts;
    for (const auto& lexeme : lexemes) {
        const auto& map = keywordLanguageMap();
        auto it = map.find(toLowerCopy(lexeme));
        if (it != map.end()) {
            counts[it->second]++;
        }
    }

    UserLanguage best = UserLanguage::Unknown;
    int best_count = 0;
    for (const auto& pair : counts) {
        if (pair.second > best_count) {
            best = pair.first;
            best_count = pair.second;
        }
    }
    return best_count == 0 ? UserLanguage::Unknown : best;
}

void reportError(ErrorCode code, UserLanguage language, int line) {
    std::string detail = errorMessageForCode(code);
    std::string message = localizedPrefix(language, detail);
    if (line > 0) {
        std::cerr << "Line " << line << ": ";
    }
    std::cerr << message << std::endl;
}

MantraError makeLexicalError(const std::string& details, int line, int col) {
    return makeError(ErrorType::LEXICAL_ERROR, ErrorCode::SYNTAX_ERROR, details, line, col);
}

MantraError makeSyntaxError(const std::string& details, int line, int col) {
    return makeError(ErrorType::SYNTAX_ERROR, ErrorCode::SYNTAX_ERROR, details, line, col);
}

MantraError makeSemanticError(const std::string& details, int line, int col) {
    return makeError(ErrorType::SEMANTIC_ERROR, ErrorCode::INVALID_OPERATION, details, line, col);
}

void printError(const MantraError& err) {
    printMantraError(err, "error");
}

void throwError(const MantraError& err) {
    printError(err);
    throw std::runtime_error(err.detail);
}

void warnError(const MantraError& err) {
    printMantraError(err, "warning");
}

void ErrorHandler::printError(ErrorType type, const std::string& message, int line, int column) {
    MantraError err;
    err.type = type;
    err.detail = message;
    err.messages = buildMessages(message);
    err.line = line;
    err.column = column;
    printMantraError(err, "error");
}

void ErrorHandler::printWarning(const std::string& message, int line, int column) {
    MantraError err;
    err.type = ErrorType::WARNING;
    err.detail = message;
    err.messages = buildMessages(message);
    err.line = line;
    err.column = column;
    printMantraError(err, "warning");
}

void ErrorHandler::throwError(ErrorType type, const std::string& message, int line, int column, int exit_code) {
    printError(type, message, line, column);
    std::exit(exit_code);
}

void ErrorHandler::setVerbosity(int level) { verbosity_level = level; }
int ErrorHandler::getVerbosity() { return verbosity_level; }

std::string ErrorHandler::formatLocation(int line, int column) {
    std::ostringstream out;
    out << "Line " << line << ", Column " << column;
    return out.str();
}

void ErrorHandler::printErrorWithContext(ErrorType type, const std::string& message,
                                          const std::string& context, int line, int column) {
    MantraError err;
    err.type = type;
    err.detail = message;
    err.messages = buildMessages(message + (context.empty() ? "" : "\n" + context));
    err.line = line;
    err.column = column;
    printMantraError(err, "error");
}

int ErrorHandler::getErrorCount() { return error_count; }
int ErrorHandler::getWarningCount() { return warning_count; }
void ErrorHandler::resetCounts() { error_count = 0; warning_count = 0; }
void ErrorHandler::incrementErrorCount() { ++error_count; }
void ErrorHandler::incrementWarningCount() { ++warning_count; }

} // namespace mantra
