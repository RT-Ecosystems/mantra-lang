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
        {"dikhao", UserLanguage::HINGLISH}, {"agar", UserLanguage::HINGLISH},
        {"kaado", UserLanguage::TANGLISH}, {"endral", UserLanguage::TANGLISH},
        {"dakho", UserLanguage::PUNGLISH}, {"jekr", UserLanguage::PUNGLISH},
        {"dekhao", UserLanguage::BENGLISH}, {"jodi", UserLanguage::BENGLISH},
        {"batavo", UserLanguage::GUJLISH}, {"jor", UserLanguage::GUJLISH},
        {"dakhav", UserLanguage::MARATHLISH}, {"jar", UserLanguage::MARATHLISH},
        {"chupinchu", UserLanguage::TELGLISH}, {"ante", UserLanguage::TELGLISH},
        {"toro", UserLanguage::KANGLISH}, {"agre", UserLanguage::KANGLISH},
        {"kaaniku", UserLanguage::MANGLISH}, {"engil", UserLanguage::MANGLISH},
        {"jadi", UserLanguage::ODLISH}, {"jabat", UserLanguage::ODLISH},
        {"nohole", UserLanguage::ASSLISH}, {"jabotloi", UserLanguage::ASSLISH},
        {"dekhaow", UserLanguage::MAITHLISH}, {"je", UserLanguage::MAITHLISH},
        {"dakhoi", UserLanguage::KONKLISH}, {"zor", UserLanguage::KONKLISH},
        {"dekhau", UserLanguage::NEPLISH}, {"yedi", UserLanguage::NEPLISH},
        {"darshaya", UserLanguage::SANGLISH}, {"yadi", UserLanguage::SANGLISH},
        {"waatav", UserLanguage::KASHLISH}, {"galaw", UserLanguage::KASHLISH},
        {"dikhay", UserLanguage::SINDLISH}, {"khoota", UserLanguage::SINDLISH},
        {"wekho", UserLanguage::DOGLISH}, {"ger", UserLanguage::DOGLISH},
        {"nungsi", UserLanguage::MANLISH}, {"amare", UserLanguage::MANLISH},
        {"nangi", UserLanguage::BODLISH}, {"bodo", UserLanguage::BODLISH},
        {"dado", UserLanguage::SANTLISH}, {"ama", UserLanguage::SANTLISH},
        {"print", UserLanguage::ENGLISH}, {"if", UserLanguage::ENGLISH},
        {"function", UserLanguage::ENGLISH}, {"return", UserLanguage::ENGLISH}
    };
    return map;
}

std::string localizedPrefix(UserLanguage language, const std::string& english) {
    switch (language) {
        case UserLanguage::HINGLISH: return "त्रुटि: " + english;
        case UserLanguage::TANGLISH: return "Pizhai: " + english;
        case UserLanguage::PUNGLISH: return "Galti: " + english;
        case UserLanguage::BENGLISH: return "Bhool: " + english;
        case UserLanguage::GUJLISH: return "Bhool: " + english;
        case UserLanguage::MARATHLISH: return "Chook: " + english;
        case UserLanguage::TELGLISH: return "Tappu: " + english;
        case UserLanguage::KANGLISH: return "Tappa: " + english;
        case UserLanguage::MANGLISH: return "Thett: " + english;
        case UserLanguage::ODLISH: return "Bhul: " + english;
        case UserLanguage::URLISH: return "Ghalat: " + english;
        case UserLanguage::ASSLISH: return "Bhool: " + english;
        case UserLanguage::MAITHLISH: return "Bhul: " + english;
        case UserLanguage::KONKLISH: return "Chuk: " + english;
        case UserLanguage::NEPLISH: return "Galti: " + english;
        case UserLanguage::SANGLISH: return "त्रुटिः: " + english;
        case UserLanguage::KASHLISH: return "Khata: " + english;
        case UserLanguage::SINDLISH: return "Galti: " + english;
        case UserLanguage::DOGLISH: return "Ghalti: " + english;
        case UserLanguage::MANLISH: return "Thabuk: " + english;
        case UserLanguage::BODLISH: return "Khontai: " + english;
        case UserLanguage::SANTLISH: return "Gomke: " + english;
        case UserLanguage::ENGLISH: return english;
        default: return english;
    }
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
             UserLanguage::HINGLISH, UserLanguage::TANGLISH, UserLanguage::PUNGLISH, UserLanguage::BENGLISH,
             UserLanguage::GUJLISH, UserLanguage::MARATHLISH, UserLanguage::TELGLISH, UserLanguage::KANGLISH,
             UserLanguage::MANGLISH, UserLanguage::ODLISH, UserLanguage::URLISH, UserLanguage::ASSLISH,
             UserLanguage::MAITHLISH, UserLanguage::KONKLISH, UserLanguage::NEPLISH, UserLanguage::SANGLISH,
             UserLanguage::KASHLISH, UserLanguage::SINDLISH, UserLanguage::DOGLISH, UserLanguage::MANLISH,
             UserLanguage::BODLISH, UserLanguage::SANTLISH, UserLanguage::ENGLISH}) {
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
    out << "\n=== MANTRA " << title << " ===\n";
    if (err.line > 0 && err.column > 0) {
        out << "Location: line " << err.line << ", col " << err.column << "\n";
    }

    auto it_en = err.messages.find(UserLanguage::ENGLISH);
    if (it_en != err.messages.end()) {
        out << it_en->second << "\n";
    } else {
        out << errorMessageForCode(err.code) << ": " << err.detail << "\n";
    }

    auto it_hi = err.messages.find(UserLanguage::HINGLISH);
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

    UserLanguage best = UserLanguage::UNKNOWN;
    int best_count = 0;
    for (const auto& pair : counts) {
        if (pair.second > best_count) {
            best = pair.first;
            best_count = pair.second;
        }
    }
    return best_count == 0 ? UserLanguage::UNKNOWN : best;
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
    err.messages = buildMessages(message + (context.empty() ? "" : ": " + context));
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
