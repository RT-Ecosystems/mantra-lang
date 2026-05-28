#pragma once

#include "ast.h"
#include "token_class.h"
#include "../error/error.h"
#include "token.h"

#include <memory>
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
    UserLanguage language;

    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;

    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool matchAny(std::initializer_list<TokenType> types);
    void skipNewlines();

    const Token& consume(TokenType type, const std::string& message);
    const Token& consumeAny(std::initializer_list<TokenType> types, const std::string& message);

    std::unique_ptr<MantraNode> declaration();
    std::unique_ptr<MantraNode> statement();
    std::unique_ptr<MantraNode> parsePrint();
    std::unique_ptr<MantraNode> parseAssignmentOrLet(bool isDeclaration);
    std::unique_ptr<MantraNode> parseIf();
    std::unique_ptr<MantraNode> parseWhile();
    std::unique_ptr<MantraNode> parseFor();
    std::unique_ptr<MantraNode> parseFunction();
    std::unique_ptr<MantraNode> parseReturn();
    std::unique_ptr<MantraNode> parseBreak();
    std::unique_ptr<MantraNode> parseContinue();
    std::unique_ptr<MantraNode> parseExpressionStatement();

    std::unique_ptr<BlockStmtNode> parseSuite();
    std::unique_ptr<BlockStmtNode> wrapBlock(std::unique_ptr<MantraNode> stmt);

    std::unique_ptr<MantraNode> expression();
    std::unique_ptr<MantraNode> logicalOr();
    std::unique_ptr<MantraNode> logicalAnd();
    std::unique_ptr<MantraNode> equality();
    std::unique_ptr<MantraNode> comparison();
    std::unique_ptr<MantraNode> term();
    std::unique_ptr<MantraNode> factor();
    std::unique_ptr<MantraNode> unary();
    std::unique_ptr<MantraNode> call();
    std::unique_ptr<MantraNode> primary();
    std::unique_ptr<MantraNode> finishCall(std::unique_ptr<MantraNode> callee);
    std::unique_ptr<MantraNode> finishIndex(std::unique_ptr<MantraNode> target);
    std::unique_ptr<MantraNode> finishMember(std::unique_ptr<MantraNode> target);

    bool isPrintKeyword(TokenType type) const;
    bool isIfKeyword(TokenType type) const;
    bool isElseKeyword(TokenType type) const;
    bool isWhileKeyword(TokenType type) const;
    bool isForKeyword(TokenType type) const;
    bool isFunctionKeyword(TokenType type) const;
    bool isReturnKeyword(TokenType type) const;
    bool isBreakKeyword(const Token& token) const;
    bool isContinueKeyword(const Token& token) const;
    bool isTrueKeyword(TokenType type) const;
    bool isFalseKeyword(TokenType type) const;
    bool isNullKeyword(const Token& token) const;
    bool isNotKeyword(TokenType type) const;
    bool isAndKeyword(TokenType type) const;
    bool isOrKeyword(TokenType type) const;
    bool isFromKeyword(TokenType type) const;
    bool isToKeyword(TokenType type) const;
    bool isThenKeyword(TokenType type) const;
    
    // NEW: Get TokenClass from current token for new grammar system
    bool checkClass(TokenClass klass) const;

    void synchronize();
    void errorAtCurrent(const std::string& message);
    void errorAt(const Token& token, const std::string& message);
};

} // namespace mantra
