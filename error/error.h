#pragma once
#include <string>
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
    WRONG_ARG_COUNT
};

enum class ErrorType {
    LEXICAL_ERROR,
    SYNTAX_ERROR,
    SEMANTIC_ERROR,
    RUNTIME_ERROR,
    TYPE_ERROR,
    IO_ERROR,
    UNKNOWN_ERROR
};

struct MantraError {
    ErrorType type;
    std::string messageEn;
    std::string messageHi;
    std::string messageTa;
    std::string messagePa;
    std::string messageBn;
    std::string messageGu;
    std::string messageMr;
    std::string messageTe;
    std::string messageKn;
    std::string messageMl;
    int line;
    int column;

    MantraError(ErrorType t,
        const std::string& en,
        const std::string& hi,
        const std::string& ta,
        const std::string& pa,
        const std::string& bn,
        const std::string& gu,
        const std::string& mr,
        const std::string& te,
        const std::string& kn,
        const std::string& ml,
        int l, int c)
        : type(t), messageEn(en), messageHi(hi),
          messageTa(ta), messagePa(pa), messageBn(bn),
          messageGu(gu), messageMr(mr), messageTe(te),
          messageKn(kn), messageMl(ml), line(l), column(c) {}
};

UserLanguage detectUserLanguage(
    const std::vector<std::string>& lexemes);

void reportError(ErrorCode code,
    UserLanguage language, int line);

MantraError makeLexicalError(
    const std::string& details, int line, int col);
MantraError makeSyntaxError(
    const std::string& details, int line, int col);
MantraError makeSemanticError(
    const std::string& details, int line, int col);

void printError(const MantraError& err);
void throwError(const MantraError& err);
void warnError(const MantraError& err);

} // namespace mantra