#pragma once

#include <exception>
#include <string>
#include <unordered_map>
#include <vector>

namespace mantra {

struct Token;

enum class UserLanguage {
    HINGLISH, TANGLISH, PUNGLISH, BENGLISH,
    GUJLISH, MARATHLISH, TELGLISH, KANGLISH,
    MANGLISH, ODLISH, URLISH, ASSLISH,
    MAITHLISH, KONKLISH, NEPLISH, SANGLISH,
    KASHLISH, SINDLISH, DOGLISH, MANLISH,
    BODLISH, SANTLISH, ENGLISH, UNKNOWN
};

enum class ErrorCode {
    MISSING_THEN,
    VAR_UNDEFINED,
    TYPE_MISMATCH,
    SYNTAX_ERROR,
    LET_MISSING_ID,
    INVALID_ASSIGN,
    MISSING_RPAREN,
    MISSING_RBRACE,
    UNDEFINED_FUNCTION,
    WRONG_ARG_COUNT,
    UNKNOWN_IDENTIFIER,
    INVALID_OPERATION,
    FILE_NOT_FOUND,
    IO_ERROR
};

enum class ErrorType {
    LEXICAL_ERROR,
    SYNTAX_ERROR,
    SEMANTIC_ERROR,
    RUNTIME_ERROR,
    TYPE_ERROR,
    UNKNOWN_IDENTIFIER,
    INVALID_OPERATION,
    FILE_NOT_FOUND,
    IO_ERROR,
    WARNING,
    INFO,
    UNKNOWN_ERROR
};

struct MantraError {
    ErrorType type = ErrorType::UNKNOWN_ERROR;
    ErrorCode code = ErrorCode::SYNTAX_ERROR;
    std::string detail;
    std::unordered_map<UserLanguage, std::string> messages;
    int line = 0;
    int column = 0;
};

UserLanguage detectUserLanguage(const std::vector<std::string>& lexemes);

void reportError(ErrorCode code, UserLanguage language, int line);

MantraError makeLexicalError(const std::string& details, int line, int col);
MantraError makeSyntaxError(const std::string& details, int line, int col);
MantraError makeSemanticError(const std::string& details, int line, int col);

void printError(const MantraError& err);
void throwError(const MantraError& err);
void warnError(const MantraError& err);

class ErrorHandler {
public:
    static void printError(ErrorType type, const std::string& message, int line, int column);
    static void printWarning(const std::string& message, int line, int column);
    static void throwError(ErrorType type, const std::string& message, int line, int column, int exit_code = 1);
    static void setVerbosity(int level);
    static int getVerbosity();
    static std::string formatLocation(int line, int column);
    static void printErrorWithContext(ErrorType type, const std::string& message,
                                      const std::string& context, int line, int column);
    static int getErrorCount();
    static int getWarningCount();
    static void resetCounts();
    static void incrementErrorCount();
    static void incrementWarningCount();
};

} // namespace mantra
