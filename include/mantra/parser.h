#pragma once
#include <memory>
#include <vector>
#include <string>
#include "token.h"
#include "ast.h"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parseProgram();

private:
    const std::vector<Token>& tokens;
    size_t current = 0;

    // Helpers
    Token peek() const;
    Token previous() const;
    bool isAtEnd() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);

    // Fixed Grammar - Assignment vs Equality
    std::unique_ptr<Expr> parseAssignment();
    std::unique_ptr<Expr> parseLogicalOr();
    std::unique_ptr<Expr> parseLogicalAnd();
    std::unique_ptr<Expr> parseEquality();
    std::unique_ptr<Expr> parseComparison();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parseFactor();
    std::unique_ptr<Expr> parseUnary();
    std::unique_ptr<Expr> parsePrimary();

    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<Stmt> parseIfStatement();
    std::unique_ptr<Stmt> parseWhileStatement();
    std::unique_ptr<Stmt> parseBlock();

    void synchronize();
    void error(Token token, const std::string& message);
};
