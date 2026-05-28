#pragma once

#include <string>

namespace mantra {

// Language identification - canonical location for UserLanguage enum
enum class UserLanguage {
    Hinglish,      // Hindi+English
    Tanglish,      // Tamil+English
    Punglish,      // Punjabi+English
    Benglish,      // Bengali+English
    Gujlish,       // Gujarati+English
    Marathlish,    // Marathi+English
    Telglish,      // Telugu+English
    Kanglish,      // Kannada+English
    Manglish,      // Malayalam+English
    Odlish,        // Odia+English
    Urlish,        // Urdu+English
    Asslish,       // Assamese+English
    Maithlish,     // Maithili+English
    Konklish,      // Konkani+English
    Neplish,       // Nepali+English
    Sanglish,      // Sanskrit+English
    Kashlish,      // Kashmiri+English
    Sindlish,      // Sindhi+English
    Doglish,       // Dogri+English
    Manlish,       // Manipuri+English
    Bodlish,       // Bodo+English
    Santlish,      // Santali+English
    English,       // Pure English
    Unknown        // Unknown/undetected language
};

// Error classifications
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

// Error codes for specific error conditions
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

// Bytecode opcodes
enum class OpCode {
    PUSH_CONST,
    ADD, SUB, MUL, DIV, MOD,
    PRINT,
    HALT,
    // Add more opcodes as needed
};

} // namespace mantra
