#include "parser.h"
#include "ast.h"
#include "error/error.h"

#include <memory>
#include <vector>
#include <stdexcept>

namespace mantra {

static UserLanguage detectLangFromTokens(
    const std::vector<Token>& tokens
) {
    std::vector<std::string> lexemes;
    for (const auto& token : tokens) {
        lexemes.push_back(token.lexeme);
    }
    return detectUserLanguage(lexemes);
}

Parser::Parser(const std::vector<Token>& toks)
    : tokens(toks),
      current(0),
      language(detectLangFromTokens(toks)) {
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EOF_TOKEN;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) { current++; }
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) { advance(); return true; }
    return false;
}

Token Parser::consume(TokenType type, ErrorCode error) {
    if (check(type)) return advance();
    reportError(error, language, peek().line);
    throw std::runtime_error("parse error");
}

std::vector<std::unique_ptr<MantraNode>>
Parser::parse() {
    std::vector<std::unique_ptr<MantraNode>> statements;
    while (!isAtEnd()) {
        if (match(TokenType::NEWLINE)) continue;
        statements.push_back(parseStatement());
    }
    return statements;
}

std::unique_ptr<MantraNode>
Parser::parseStatement() {
    if (match(TokenType::PRINT)) return parsePrint();
    if (match(TokenType::LET)) return parseLet();
    if (match(TokenType::IF)) return parseIf();
    if (match(TokenType::WHILE)) return parseWhile();
    if (match(TokenType::FOR)) return parseFor();
    if (match(TokenType::FUNCTION)) return parseFunction();
    if (match(TokenType::RETURN)) return parseReturn();
    if (match(TokenType::LBRACE)) return parseBlock();
    if (check(TokenType::IDENTIFIER) &&
        current + 1 < tokens.size() &&
        tokens[current + 1].type == TokenType::EQUALS)
        return parseAssignment();
    return parseExprStmt();
}

std::unique_ptr<MantraNode>
Parser::parsePrint() {
    if (match(TokenType::EQUALS)) {}
    auto expr = parseExpression();
    return std::make_unique<PrintStmtNode>(std::move(expr));
}

std::unique_ptr<MantraNode>
Parser::parseLet() {
    Token name = consume(TokenType::IDENTIFIER,
        ErrorCode::LET_MISSING_ID);
    consume(TokenType::EQUALS, ErrorCode::INVALID_ASSIGN);
    auto value = parseExpression();
    return std::make_unique<AssignStmtNode>(
        name.lexeme, std::move(value));
}

std::unique_ptr<MantraNode>
Parser::parseAssignment() {
    Token name = consume(TokenType::IDENTIFIER,
        ErrorCode::INVALID_ASSIGN);
    consume(TokenType::EQUALS, ErrorCode::INVALID_ASSIGN);
    auto value = parseExpression();
    return std::make_unique<AssignStmtNode>(
        name.lexeme, std::move(value));
}

std::unique_ptr<MantraNode>
Parser::parseIf() {
    auto condition = parseExpression();
    if (match(TokenType::THEN)) {}
    auto thenBranch = parseStatement();
    std::unique_ptr<MantraNode> elseBranch = nullptr;
    if (match(TokenType::ELSE))
        elseBranch = parseStatement();
    return std::make_unique<IfStmtNode>(
        std::move(condition),
        std::move(thenBranch),
        std::move(elseBranch));
}

std::unique_ptr<MantraNode>
Parser::parseWhile() {
    auto condition = parseExpression();
    auto body = parseStatement();
    return std::make_unique<WhileStmtNode>(
        std::move(condition), std::move(body));
}

std::unique_ptr<MantraNode>
Parser::parseFor() {
    Token variable = consume(TokenType::IDENTIFIER,
        ErrorCode::SYNTAX_ERROR);
    consume(TokenType::EQUALS, ErrorCode::INVALID_ASSIGN);
    auto startExpr = parseExpression();
    consume(TokenType::FROM, ErrorCode::SYNTAX_ERROR);
    auto endExpr = parseExpression();
    consume(TokenType::TO, ErrorCode::SYNTAX_ERROR);
    auto body = parseStatement();
    return std::make_unique<ForStmtNode>(
        variable.lexeme,
        std::move(startExpr),
        std::move(endExpr),
        std::move(body));
}

std::unique_ptr<MantraNode>
Parser::parseFunction() {
    Token name = consume(TokenType::IDENTIFIER,
        ErrorCode::SYNTAX_ERROR);
    consume(TokenType::LPAREN, ErrorCode::MISSING_RPAREN);
    std::vector<std::string> params;
    if (!check(TokenType::RPAREN)) {
        while (true) {
            Token param = consume(TokenType::IDENTIFIER,
                ErrorCode::SYNTAX_ERROR);
            params.push_back(param.lexeme);
            if (match(TokenType::COMMA)) continue;
            break;
        }
    }
    consume(TokenType::RPAREN, ErrorCode::MISSING_RPAREN);
    std::unique_ptr<MantraNode> body;
    if (match(TokenType::LBRACE))
        body = parseBlock();
    else
        body = parseStatement();
    return std::make_unique<FuncDefNode>(
        name.lexeme, params, std::move(body));
}

std::unique_ptr<MantraNode>
Parser::parseReturn() {
    auto value = parseExpression();
    return std::make_unique<ReturnStmtNode>(std::move(value));
}

std::unique_ptr<MantraNode>
Parser::parseBlock() {
    std::vector<std::unique_ptr<MantraNode>> statements;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (match(TokenType::NEWLINE)) continue;
        statements.push_back(parseStatement());
    }
    consume(TokenType::RBRACE, ErrorCode::MISSING_RBRACE);
    return std::make_unique<BlockStmtNode>(std::move(statements));
}

std::unique_ptr<MantraNode>
Parser::parseExprStmt() {
    auto expr = parseExpression();
    return std::make_unique<ExprStmtNode>(std::move(expr));
}

std::unique_ptr<MantraNode>
Parser::parseExpression() { return parseOr(); }

std::unique_ptr<MantraNode>
Parser::parseOr() {
    auto expr = parseAnd();
    while (match(TokenType::OR)) {
        Token op = previous();
        auto right = parseAnd();
        expr = std::make_unique<BinaryExprNode>(
            std::move(expr), op.lexeme, std::move(right));
    }
    return expr;
}

std::unique_ptr<MantraNode>
Parser::parseAnd() {
    auto expr = parseNot();
    while (match(TokenType::AND)) {
        Token op = previous();
        auto right = parseNot();
        expr = std::make_unique<BinaryExprNode>(
            std::move(expr), op.lexeme, std::move(right));
    }
    return expr;
}

std::unique_ptr<MantraNode>
Parser::parseNot() {
    if (match(TokenType::NOT)) {
        Token op = previous();
        auto right = parseNot();
        return std::make_unique<UnaryExprNode>(
            op.lexeme, std::move(right));
    }
    return parseEquality();
}

std::unique_ptr<MantraNode>
Parser::parseEquality() {
    auto expr = parseComparison();
    while (match(TokenType::EQEQ) || match(TokenType::NEQ)) {
        Token op = previous();
        auto right = parseComparison();
        expr = std::make_unique<BinaryExprNode>(
            std::move(expr), op.lexeme, std::move(right));
    }
    return expr;
}

std::unique_ptr<MantraNode>
Parser::parseComparison() {
    auto expr = parseTerm();
    while (match(TokenType::LT) || match(TokenType::GT) ||
           match(TokenType::LTE) || match(TokenType::GTE)) {
        Token op = previous();
        auto right = parseTerm();
        expr = std::make_unique<BinaryExprNode>(
            std::move(expr), op.lexeme, std::move(right));
    }
    return expr;
}

std::unique_ptr<MantraNode>
Parser::parseTerm() {
    auto expr = parseFactor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = previous();
        auto right = parseFactor();
        expr = std::make_unique<BinaryExprNode>(
            std::move(expr), op.lexeme, std::move(right));
    }
    return expr;
}

std::unique_ptr<MantraNode>
Parser::parseFactor() {
    auto expr = parseUnary();
    while (match(TokenType::STAR) || match(TokenType::SLASH)) {
        Token op = previous();
        auto right = parseUnary();
        expr = std::make_unique<BinaryExprNode>(
            std::move(expr), op.lexeme, std::move(right));
    }
    return expr;
}

std::unique_ptr<MantraNode>
Parser::parseUnary() {
    if (match(TokenType::MINUS) || match(TokenType::NOT)) {
        Token op = previous();
        auto right = parseUnary();
        return std::make_unique<UnaryExprNode>(
            op.lexeme, std::move(right));
    }
    return parseCall();
}

std::unique_ptr<MantraNode>
Parser::parseCall() {
    auto expr = parsePrimary();
    while (true) {
        if (match(TokenType::LPAREN)) {
            std::vector<std::unique_ptr<MantraNode>> args;
            if (!check(TokenType::RPAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RPAREN, ErrorCode::MISSING_RPAREN);
            expr = std::make_unique<CallExprNode>(
                std::move(expr), std::move(args));
        } else { break; }
    }
    return expr;
}

std::unique_ptr<MantraNode>
Parser::parsePrimary() {
    if (match(TokenType::NUMBER))
        return std::make_unique<NumberLitNode>(
            std::stod(previous().lexeme));
    if (match(TokenType::STRING))
        return std::make_unique<StringLitNode>(
            previous().lexeme);
    if (match(TokenType::TRUE))
        return std::make_unique<BoolLitNode>(true);
    if (match(TokenType::FALSE))
        return std::make_unique<BoolLitNode>(false);
    if (match(TokenType::IDENTIFIER))
        return std::make_unique<IdentifierNode>(
            previous().lexeme);
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, ErrorCode::MISSING_RPAREN);
        return expr;
    }
    reportError(ErrorCode::SYNTAX_ERROR,
        language, peek().line);
    throw std::runtime_error("parse error");
}

} // namespace mantra