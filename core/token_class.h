#ifndef MANTRA_TOKEN_CLASS_H
#define MANTRA_TOKEN_CLASS_H

#include <string>

namespace mantra {

/**
 * TokenClass: Canonical token classification system
 *
 * Instead of 200+ TokenType variants, MANTRA uses ~25 canonical TokenClass entries.
 * Each TokenClass represents a semantic category, mapping to multiple language variants.
 */
enum class TokenClass {
    // === CONTROL FLOW STATEMENTS ===
    PRINT,                  // All print keywords (dikhao, kaado, dakho, ...)
    IF,                     // All if keywords (agar, endral, jekr, ...)
    ELSE,                   // All else keywords (warna, illena, nahole, ...)
    WHILE,                  // All while keywords (baarbaar, marumurai, ...)
    FOR,                    // All for keywords (baarbaar, for)
    FUNCTION,               // All function keywords (kaam, seyal, ...)
    RETURN,                 // All return keywords (wapas, thirumbu, ...)
    BREAK,                  // break
    CONTINUE,               // continue
    LET,                    // All let keywords (rakho, let)

    // === LITERALS & IDENTIFIERS ===
    TRUE,                   // All true keywords (sach, unmai, satya, ...)
    FALSE,                  // All false keywords (jhooth, poi, mittha, ...)
    NULL_LIT,               // null
    NUMBER,                 // Integer or float: 123, 45.67, 0xFF
    STRING,                 // String literal: "text", 'text'
    IDENTIFIER,             // Variable/function names: x, my_var, result

    // === ARITHMETIC OPERATORS ===
    PLUS,                   // + (addition)
    MINUS,                  // - (subtraction)
    MULTIPLY,               // * (multiplication)
    DIVIDE,                 // / (division)
    MODULO,                 // % (modulo)

    // === COMPARISON OPERATORS ===
    EQUAL,                  // == (equality test)
    NOT_EQUAL,              // != (inequality test)
    LESS,                   // < (less than)
    LESS_EQUAL,             // <= (less than or equal)
    GREATER,                // > (greater than)
    GREATER_EQUAL,          // >= (greater than or equal)

    // === LOGICAL OPERATORS ===
    AND,                    // && or keyword versions (aur, and, mattrum, ...)
    OR,                     // || or keyword versions (ya, or, kiva, ...)
    NOT,                    // ! or keyword versions (nahi, not)

    // === ASSIGNMENT & CONTROL ===
    ASSIGN,                 // = (assignment operator)
    THEN,                   // then/tab (conditional marker)
    FROM,                   // from/se (loop range start)
    TO,                     // to/tak (loop range end)

    // === DELIMITERS ===
    LPAREN,                 // ( (left parenthesis)
    RPAREN,                 // ) (right parenthesis)
    LBRACE,                 // { (left brace)
    RBRACE,                 // } (right brace)
    LBRACKET,               // [ (left bracket)
    RBRACKET,               // ] (right bracket)
    COMMA,                  // , (comma)
    DOT,                    // . (member access)
    SEMICOLON,              // ; (statement terminator)
    COLON,                  // : (type annotation or dict separator)

    // === SPECIAL TOKENS ===
    NEWLINE,                // Newline character(s)
    EOF_TOKEN,              // End of file marker
    UNKNOWN,                // Unknown token (error)
    ERROR_TOKEN             // Explicit error token
};

/**
 * Convert TokenClass to human-readable string
 */
inline std::string tokenClassToString(TokenClass klass) {
    switch (klass) {
        case TokenClass::PRINT: return "PRINT";
        case TokenClass::IF: return "IF";
        case TokenClass::ELSE: return "ELSE";
        case TokenClass::WHILE: return "WHILE";
        case TokenClass::FOR: return "FOR";
        case TokenClass::FUNCTION: return "FUNCTION";
        case TokenClass::RETURN: return "RETURN";
        case TokenClass::BREAK: return "BREAK";
        case TokenClass::CONTINUE: return "CONTINUE";
        case TokenClass::LET: return "LET";
        case TokenClass::TRUE: return "TRUE";
        case TokenClass::FALSE: return "FALSE";
        case TokenClass::NULL_LIT: return "NULL";
        case TokenClass::NUMBER: return "NUMBER";
        case TokenClass::STRING: return "STRING";
        case TokenClass::IDENTIFIER: return "IDENTIFIER";
        case TokenClass::PLUS: return "PLUS";
        case TokenClass::MINUS: return "MINUS";
        case TokenClass::MULTIPLY: return "MULTIPLY";
        case TokenClass::DIVIDE: return "DIVIDE";
        case TokenClass::MODULO: return "MODULO";
        case TokenClass::EQUAL: return "EQUAL";
        case TokenClass::NOT_EQUAL: return "NOT_EQUAL";
        case TokenClass::LESS: return "LESS";
        case TokenClass::LESS_EQUAL: return "LESS_EQUAL";
        case TokenClass::GREATER: return "GREATER";
        case TokenClass::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenClass::AND: return "AND";
        case TokenClass::OR: return "OR";
        case TokenClass::NOT: return "NOT";
        case TokenClass::ASSIGN: return "ASSIGN";
        case TokenClass::THEN: return "THEN";
        case TokenClass::FROM: return "FROM";
        case TokenClass::TO: return "TO";
        case TokenClass::LPAREN: return "LPAREN";
        case TokenClass::RPAREN: return "RPAREN";
        case TokenClass::LBRACE: return "LBRACE";
        case TokenClass::RBRACE: return "RBRACE";
        case TokenClass::LBRACKET: return "LBRACKET";
        case TokenClass::RBRACKET: return "RBRACKET";
        case TokenClass::COMMA: return "COMMA";
        case TokenClass::DOT: return "DOT";
        case TokenClass::SEMICOLON: return "SEMICOLON";
        case TokenClass::COLON: return "COLON";
        case TokenClass::NEWLINE: return "NEWLINE";
        case TokenClass::EOF_TOKEN: return "EOF";
        case TokenClass::UNKNOWN: return "UNKNOWN";
        case TokenClass::ERROR_TOKEN: return "ERROR";
        default: return "UNKNOWN_CLASS";
    }
}

/**
 * Get operator precedence level (1=lowest, 7=highest)
 */
inline int getPrecedence(TokenClass klass) {
    switch (klass) {
        case TokenClass::OR: return 1;
        case TokenClass::AND: return 2;
        case TokenClass::EQUAL:
        case TokenClass::NOT_EQUAL: return 3;
        case TokenClass::LESS:
        case TokenClass::LESS_EQUAL:
        case TokenClass::GREATER:
        case TokenClass::GREATER_EQUAL: return 4;
        case TokenClass::PLUS:
        case TokenClass::MINUS: return 5;
        case TokenClass::MULTIPLY:
        case TokenClass::DIVIDE:
        case TokenClass::MODULO: return 6;
        case TokenClass::NOT: return 7;
        default: return 0;
    }
}

/**
 * Associativity direction for operators
 */
enum class Associativity {
    LEFT,    // Most binary operators
    RIGHT    // Unary operators
};

/**
 * Get operator associativity
 */
inline Associativity getAssociativity(TokenClass klass) {
    switch (klass) {
        case TokenClass::NOT:
            return Associativity::RIGHT;
        default:
            return Associativity::LEFT;
    }
}

} // namespace mantra

#endif // MANTRA_TOKEN_CLASS_H
