#ifndef MANTRA_PARSER_H
#define MANTRA_PARSER_H

#include "ast.h"
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
    const Token& peekNextNonNewline(size_t start, size_t& out_index) const;
    bool isAtEnd() const;

    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool matchAny(const std::vector<TokenType>& types);

    bool skipSeparators();
    void skipNewlines();

    int peekIndent() const;
    void parseIndentedStatements(std::vector<std::unique_ptr<MantraNode>>& out, int indent_level);

    std::unique_ptr<MantraNode> declaration(int indent_level);
    std::unique_ptr<MantraNode> statement(int indent_level);

    std::unique_ptr<MantraNode> printStatement(const Token& keyword);
    std::unique_ptr<MantraNode> ifStatement(const Token& keyword, int indent_level);
    std::unique_ptr<MantraNode> whileStatement(const Token& keyword, int indent_level);
    std::unique_ptr<MantraNode> forStatement(const Token& keyword, int indent_level);
    std::unique_ptr<MantraNode> functionStatement(const Token& keyword, int indent_level);
    std::unique_ptr<MantraNode> returnStatement(const Token& keyword);
    std::unique_ptr<MantraNode> breakStatement(const Token& keyword);
    std::unique_ptr<MantraNode> variableDeclaration(const Token& keyword);
    std::unique_ptr<MantraNode> expressionStatement();

    std::unique_ptr<BlockStmtNode> braceBlock(const Token& owner);
    std::unique_ptr<BlockStmtNode> indentBlock(const Token& owner, int indent_level);
    std::unique_ptr<BlockStmtNode> blockOrSingle(const Token& owner, int indent_level);

    std::unique_ptr<MantraNode> expression();
    std::unique_ptr<MantraNode> assignment();
    std::unique_ptr<MantraNode> logicalOr();
    std::unique_ptr<MantraNode> logicalAnd();
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
    bool isAndKeyword(TokenType type) const;
    bool isOrKeyword(TokenType type) const;
    bool isFromKeyword(TokenType type) const;
    bool isToKeyword(TokenType type) const;
    bool isThenKeyword(TokenType type) const;

    bool looksLikeForLoop() const;
    bool isStepToken(const Token& token) const;
    bool isBreakToken(const Token& token) const;

    void synchronizeLine();
    void errorAtCurrent(const std::string& message);
    void errorAt(const Token& token, const std::string& message);
};

} // namespace mantra

#endif // MANTRA_PARSER_H
