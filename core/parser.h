#ifndef MANTRA_PARSER_H
#define MANTRA_PARSER_H

#include "ast.h"
#include "lexer.h"
#include "token.h"
#include <memory>
#include <string>
#include <vector>

namespace mantra {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    std::unique_ptr<ProgramNode> parseProgram();

    bool hasError() const { return had_error; }

private:
    const std::vector<Token>& tokens;
    size_t current;
    bool had_error;

    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;

    const Token& advance();
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    bool match(TokenType type);

    void skipNewlines();

    std::unique_ptr<MantraNode> declaration();
    std::unique_ptr<MantraNode> statement();
    std::unique_ptr<MantraNode> printStatement();
    std::unique_ptr<MantraNode> ifStatement();
    std::unique_ptr<MantraNode> whileStatement();
    std::unique_ptr<MantraNode> forStatement();
    std::unique_ptr<MantraNode> functionStatement();
    std::unique_ptr<MantraNode> returnStatement();
    std::unique_ptr<MantraNode> expressionStatement();

    std::unique_ptr<BlockNode> block();
    std::unique_ptr<BlockNode> blockOrSingle();

    std::unique_ptr<MantraNode> assignment();
    std::unique_ptr<MantraNode> expression();
    std::unique_ptr<MantraNode> equality();
    std::unique_ptr<MantraNode> comparison();
    std::unique_ptr<MantraNode> term();
    std::unique_ptr<MantraNode> factor();
    std::unique_ptr<MantraNode> unary();
    std::unique_ptr<MantraNode> call();
    std::unique_ptr<MantraNode> primary();

    bool isPrintKeyword(TokenType type) const;
    bool isIfKeyword(TokenType type) const;
    bool isElseKeyword(TokenType type) const;
    bool isWhileKeyword(TokenType type) const;
    bool isForKeyword(TokenType type) const;
    bool isFunctionKeyword(TokenType type) const;
    bool isReturnKeyword(TokenType type) const;
    bool isTrueKeyword(TokenType type) const;
    bool isFalseKeyword(TokenType type) const;
    bool isNotKeyword(TokenType type) const;
    bool isFromKeyword(TokenType type) const;
    bool isToKeyword(TokenType type) const;
    bool isThenKeyword(TokenType type) const;

    void synchronize();
    void errorAtCurrent(const std::string& message);
    void errorAt(const Token& token, const std::string& message);
};

} // namespace mantra

#endif // MANTRA_PARSER_H
