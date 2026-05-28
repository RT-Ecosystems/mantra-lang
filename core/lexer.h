#ifndef MANTRA_LEXER_H
#define MANTRA_LEXER_H

#include "token.h"
#include "token_class.h"
#include "keyword_normalizer.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace mantra {

class Lexer {
public:
    // Constructor that takes source code as string
    explicit Lexer(const std::string& source);
    
    // Destructor
    ~Lexer() = default;
    
    // Tokenize the entire source and return vector of tokens
    std::vector<Token> tokenize();
    
    // Get next token
    Token nextToken();
    
    // Peek at next token without consuming it
    Token peekToken();
    
    // Get all tokens
    const std::vector<Token>& getTokens() const { return tokens; }
    
    // Get current line
    int getCurrentLine() const { return line; }
    
    // Get current column
    int getCurrentColumn() const { return column; }
    
    // Pretty print all tokens
    void printTokens() const;

private:
    std::string source;
    std::vector<Token> tokens;
    size_t position;
    int line;
    int column;
    size_t line_start;
    
    // Keyword normalization system
    static KeywordNormalizer keyword_normalizer;
    
    // Keyword map for all 22 Indian language mixes
    static const std::unordered_map<std::string, TokenType> keyword_map;
    
    // Helper methods
    char currentChar();
    char peekChar(int offset = 1);
    char nextChar();
    void skipWhitespace();
    void skipComment();
    
    // Tokenization helpers
    Token readString(char quote);
    Token readTripleQuoteString(char quote);
    Token readNumber();
    Token readIdentifierOrKeyword();
    
    // Character classification
    bool isWhitespace(char ch);
    bool isNewline(char ch);
    bool isDigit(char ch);
    bool isHexDigit(char ch);
    bool isAlpha(char ch);
    bool isAlphaNumeric(char ch);
    bool isOperator(char ch);
    
    // Error reporting
    void reportError(const std::string& message);
};

} // namespace mantra

#endif // MANTRA_LEXER_H
