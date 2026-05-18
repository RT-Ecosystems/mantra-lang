#include "parser.h"

#include "../error/error.h"

#include <algorithm>
#include <cctype>
#include <initializer_list>
#include <stdexcept>

namespace mantra {
namespace {

std::string toLowerCopy(const std::string& value) {
    std::string out;
    out.reserve(value.size());
    for (char ch : value) {
        out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
    }
    return out;
}

template <typename T>
bool containsTokenType(std::initializer_list<T> types, T value) {
    return std::find(types.begin(), types.end(), value) != types.end();
}

} // namespace

Parser::Parser(const std::vector<Token>& toks)
    : tokens(toks), current(0), had_error(false), language(UserLanguage::UNKNOWN) {
    std::vector<std::string> lexemes;
    lexemes.reserve(tokens.size());
    for (const auto& token : tokens) {
        lexemes.push_back(token.lexeme);
    }
    language = detectUserLanguage(lexemes);
}

const Token& Parser::peek() const { return tokens[current]; }
const Token& Parser::previous() const { return tokens[current - 1]; }
bool Parser::isAtEnd() const { return peek().type == TokenType::EOF_TOKEN; }

const Token& Parser::advance() {
    if (!isAtEnd()) {
        ++current;
    }
    return previous();
}

bool Parser::check(TokenType type) const {
    return !isAtEnd() && peek().type == type;
}

bool Parser::match(TokenType type) {
    if (!check(type)) {
        return false;
    }
    advance();
    return true;
}

bool Parser::matchAny(std::initializer_list<TokenType> types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void Parser::skipNewlines() {
    while (match(TokenType::NEWLINE)) {}
}

const Token& Parser::consume(TokenType type, ErrorCode code) {
    if (check(type)) {
        return advance();
    }
    reportError(code, language, peek().line);
    had_error = true;
    throw std::runtime_error("parse error");
}

const Token& Parser::consumeAny(std::initializer_list<TokenType> types, ErrorCode code) {
    for (auto type : types) {
        if (check(type)) {
            return advance();
        }
    }
    reportError(code, language, peek().line);
    had_error = true;
    throw std::runtime_error("parse error");
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>(1, 1);
    while (!isAtEnd()) {
        skipNewlines();
        if (isAtEnd()) {
            break;
        }
        try {
            auto stmt = declaration();
            if (stmt) {
                program->statements.push_back(std::move(stmt));
            }
        } catch (const std::exception&) {
            synchronize();
        }
        skipNewlines();
    }
    return program;
}

std::unique_ptr<MantraNode> Parser::declaration() {
    return statement();
}

std::unique_ptr<MantraNode> Parser::statement() {
    skipNewlines();
    if (isPrintKeyword(peek().type)) {
        return parsePrint();
    }
    if (matchAny({TokenType::KW_LET, TokenType::KW_RAKHO})) {
        return parseAssignmentOrLet();
    }
    if (isIfKeyword(peek().type)) {
        return parseIf();
    }
    if (isWhileKeyword(peek().type)) {
        return parseWhile();
    }
    if (isForKeyword(peek().type)) {
        return parseFor();
    }
    if (isFunctionKeyword(peek().type)) {
        return parseFunction();
    }
    if (isReturnKeyword(peek().type)) {
        return parseReturn();
    }
    if (isBreakKeyword(peek())) {
        return parseBreak();
    }
    if (match(TokenType::LBRACE)) {
        auto suite = std::make_unique<BlockStmtNode>(previous().line, previous().column);
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            skipNewlines();
            if (check(TokenType::RBRACE) || isAtEnd()) {
                break;
            }
            suite->statements.push_back(declaration());
            skipNewlines();
        }
        consume(TokenType::RBRACE, ErrorCode::MISSING_RBRACE);
        return suite;
    }
    if (check(TokenType::IDENTIFIER) && current + 1 < tokens.size() &&
        tokens[current + 1].type == TokenType::OP_ASSIGN) {
        return parseAssignmentOrLet();
    }
    return parseExpressionStatement();
}

std::unique_ptr<MantraNode> Parser::parsePrint() {
    const Token& keyword = advance();
    match(TokenType::OP_ASSIGN);
    auto expr = expression();
    return std::make_unique<PrintStmtNode>(std::move(expr), keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::parseAssignmentOrLet() {
    const Token& keyword = previous();
    Token name = consume(TokenType::IDENTIFIER, ErrorCode::LET_MISSING_ID);
    consume(TokenType::OP_ASSIGN, ErrorCode::INVALID_ASSIGN);
    auto value = expression();
    (void)keyword;
    return std::make_unique<AssignStmtNode>(name.lexeme, std::move(value), name.line, name.column);
}

std::unique_ptr<MantraNode> Parser::parseIf() {
    const Token& keyword = advance();
    auto condition = expression();
    if (isThenKeyword(peek().type)) {
        advance();
    }
    auto thenBranch = parseSuite();
    std::unique_ptr<BlockStmtNode> elseBranch;
    skipNewlines();
    if (isElseKeyword(peek().type)) {
        advance();
        elseBranch = parseSuite();
    }
    return std::make_unique<IfStmtNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch),
                                        keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::parseWhile() {
    const Token& keyword = advance();
    auto condition = expression();
    auto body = parseSuite();
    return std::make_unique<WhileStmtNode>(std::move(condition), std::move(body), keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::parseFor() {
    const Token& keyword = advance();
    Token variable = consume(TokenType::IDENTIFIER, ErrorCode::SYNTAX_ERROR);
    if (match(TokenType::OP_ASSIGN)) {
        // optional equals
    }
    auto start = expression();
    consumeAny({TokenType::KW_FROM, TokenType::KW_SE}, ErrorCode::SYNTAX_ERROR);
    auto end = expression();
    matchAny({TokenType::KW_TO, TokenType::KW_TAK});
    auto body = parseSuite();
    return std::make_unique<ForStmtNode>(variable.lexeme, std::move(start), std::move(end), nullptr,
                                         std::move(body), keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::parseFunction() {
    const Token& keyword = advance();
    Token name = consume(TokenType::IDENTIFIER, ErrorCode::SYNTAX_ERROR);
    consume(TokenType::LPAREN, ErrorCode::MISSING_RPAREN);
    std::vector<std::string> params;
    if (!check(TokenType::RPAREN)) {
        do {
            Token param = consume(TokenType::IDENTIFIER, ErrorCode::SYNTAX_ERROR);
            params.push_back(param.lexeme);
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RPAREN, ErrorCode::MISSING_RPAREN);
    auto body = parseSuite();
    return std::make_unique<FuncDefNode>(name.lexeme, std::move(params), std::move(body), keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::parseReturn() {
    const Token& keyword = advance();
    if (check(TokenType::NEWLINE) || check(TokenType::RBRACE) || isElseKeyword(peek().type) || isAtEnd()) {
        return std::make_unique<ReturnStmtNode>(nullptr, keyword.line, keyword.column);
    }
    auto value = expression();
    return std::make_unique<ReturnStmtNode>(std::move(value), keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::parseBreak() {
    const Token& token = advance();
    return std::make_unique<BreakStmtNode>(token.line, token.column);
}

std::unique_ptr<MantraNode> Parser::parseExpressionStatement() {
    auto expr = expression();
    const int line = expr ? expr->line : peek().line;
    const int column = expr ? expr->column : peek().column;
    return std::make_unique<ExprStmtNode>(std::move(expr), line, column);
}

std::unique_ptr<BlockStmtNode> Parser::parseSuite() {
    skipNewlines();
    if (match(TokenType::LBRACE)) {
        auto block = std::make_unique<BlockStmtNode>(previous().line, previous().column);
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            skipNewlines();
            if (check(TokenType::RBRACE) || isAtEnd()) {
                break;
            }
            block->statements.push_back(declaration());
            skipNewlines();
        }
        consume(TokenType::RBRACE, ErrorCode::MISSING_RBRACE);
        return block;
    }

    auto block = std::make_unique<BlockStmtNode>(peek().line, peek().column);
    if (!isAtEnd() && !check(TokenType::NEWLINE) && !isElseKeyword(peek().type)) {
        block->statements.push_back(declaration());
    }
    return block;
}

std::unique_ptr<BlockStmtNode> Parser::wrapBlock(std::unique_ptr<MantraNode> stmt) {
    auto block = std::make_unique<BlockStmtNode>(stmt ? stmt->line : peek().line,
                                                 stmt ? stmt->column : peek().column);
    if (stmt) {
        block->statements.push_back(std::move(stmt));
    }
    return block;
}

std::unique_ptr<MantraNode> Parser::expression() {
    return logicalOr();
}

std::unique_ptr<MantraNode> Parser::logicalOr() {
    auto expr = logicalAnd();
    while (isOrKeyword(peek().type)) {
        Token op = advance();
        auto right = logicalAnd();
        expr = std::make_unique<BinaryExprNode>(TokenType::OP_OR, std::move(expr), std::move(right), op.line, op.column);
    }
    return expr;
}

std::unique_ptr<MantraNode> Parser::logicalAnd() {
    auto expr = equality();
    while (isAndKeyword(peek().type)) {
        Token op = advance();
        auto right = equality();
        expr = std::make_unique<BinaryExprNode>(TokenType::OP_AND, std::move(expr), std::move(right), op.line, op.column);
    }
    return expr;
}

std::unique_ptr<MantraNode> Parser::equality() {
    auto expr = comparison();
    while (matchAny({TokenType::OP_EQUAL, TokenType::OP_NOT_EQUAL})) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExprNode>(op.type, std::move(expr), std::move(right), op.line, op.column);
    }
    return expr;
}

std::unique_ptr<MantraNode> Parser::comparison() {
    auto expr = term();
    while (matchAny({TokenType::OP_LESS, TokenType::OP_LESS_EQUAL, TokenType::OP_GREATER, TokenType::OP_GREATER_EQUAL})) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<BinaryExprNode>(op.type, std::move(expr), std::move(right), op.line, op.column);
    }
    return expr;
}

std::unique_ptr<MantraNode> Parser::term() {
    auto expr = factor();
    while (matchAny({TokenType::OP_PLUS, TokenType::OP_MINUS})) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExprNode>(op.type, std::move(expr), std::move(right), op.line, op.column);
    }
    return expr;
}

std::unique_ptr<MantraNode> Parser::factor() {
    auto expr = unary();
    while (matchAny({TokenType::OP_MULTIPLY, TokenType::OP_DIVIDE, TokenType::OP_MODULO})) {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<BinaryExprNode>(op.type, std::move(expr), std::move(right), op.line, op.column);
    }
    return expr;
}

std::unique_ptr<MantraNode> Parser::unary() {
    if (matchAny({TokenType::OP_MINUS, TokenType::OP_NOT, TokenType::KW_NOT, TokenType::KW_NAHI})) {
        Token op = previous();
        auto right = unary();
        TokenType normalized = op.type == TokenType::OP_MINUS ? TokenType::OP_MINUS : TokenType::OP_NOT;
        return std::make_unique<UnaryExprNode>(normalized, std::move(right), op.line, op.column);
    }
    return call();
}

std::unique_ptr<MantraNode> Parser::call() {
    auto expr = primary();
    while (true) {
        if (match(TokenType::LPAREN)) {
            expr = finishCall(std::move(expr));
        } else if (match(TokenType::LBRACKET)) {
            expr = finishIndex(std::move(expr));
        } else {
            break;
        }
    }
    return expr;
}

std::unique_ptr<MantraNode> Parser::finishCall(std::unique_ptr<MantraNode> callee) {
    std::vector<std::unique_ptr<MantraNode>> args;
    if (!check(TokenType::RPAREN)) {
        do {
            args.push_back(expression());
        } while (match(TokenType::COMMA));
    }
    Token close = consume(TokenType::RPAREN, ErrorCode::MISSING_RPAREN);
    return std::make_unique<CallExprNode>(std::move(callee), std::move(args), close.line, close.column);
}

std::unique_ptr<MantraNode> Parser::finishIndex(std::unique_ptr<MantraNode> target) {
    auto idx = expression();
    Token close = consume(TokenType::RBRACKET, ErrorCode::SYNTAX_ERROR);
    return std::make_unique<IndexExprNode>(std::move(target), std::move(idx), close.line, close.column);
}

std::unique_ptr<MantraNode> Parser::primary() {
    if (matchAny({TokenType::NUMBER_INT, TokenType::NUMBER_FLOAT, TokenType::NUMBER_HEX})) {
        const Token& tok = previous();
        return std::make_unique<NumberLitNode>(std::stod(tok.lexeme), true, tok.lexeme, tok.line, tok.column);
    }
    if (match(TokenType::STRING)) {
        const Token& tok = previous();
        return std::make_unique<StringLitNode>(tok.lexeme, tok.line, tok.column);
    }
    if (matchAny({TokenType::KW_TRUE, TokenType::KW_SACH, TokenType::KW_UNMAI, TokenType::KW_SATYA, TokenType::KW_SACHU,
                  TokenType::KW_SATHYAM, TokenType::KW_BEO, TokenType::KW_BAE_HON, TokenType::KW_SANO, TokenType::KW_NIJAM,
                  TokenType::KW_KHOTA, TokenType::KW_KHOOTA, TokenType::KW_NIJHA, TokenType::KW_THAMBAL, TokenType::KW_NIJAM,
                  TokenType::KW_SACH, TokenType::KW_KHOTA})) {
        const Token& tok = previous();
        return std::make_unique<BoolLitNode>(true, tok.line, tok.column);
    }
    if (matchAny({TokenType::KW_FALSE, TokenType::KW_JHOOTH, TokenType::KW_POI, TokenType::KW_MITTHA, TokenType::KW_JUTHU,
                  TokenType::KW_ABADDHAM, TokenType::KW_SULLU, TokenType::KW_ASATHYAM, TokenType::KW_MAKHAL, TokenType::KW_NOA,
                  TokenType::KW_MITHA, TokenType::KW_KHOOTA})) {
        const Token& tok = previous();
        return std::make_unique<BoolLitNode>(false, tok.line, tok.column);
    }
    if (match(TokenType::LBRACKET)) {
        const Token& open = previous();
        std::vector<std::unique_ptr<MantraNode>> elements;
        if (!check(TokenType::RBRACKET)) {
            do {
                elements.push_back(expression());
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RBRACKET, ErrorCode::SYNTAX_ERROR);
        return std::make_unique<ArrayLitNode>(std::move(elements), open.line, open.column);
    }
    if (match(TokenType::LPAREN)) {
        auto expr = expression();
        consume(TokenType::RPAREN, ErrorCode::MISSING_RPAREN);
        return expr;
    }
    if (match(TokenType::IDENTIFIER)) {
        const Token& tok = previous();
        if (toLowerCopy(tok.lexeme) == "null") {
            return std::make_unique<NullLitNode>(tok.line, tok.column);
        }
        return std::make_unique<IdentifierNode>(tok.lexeme, tok.line, tok.column);
    }

    errorAtCurrent("Unexpected token");
    throw std::runtime_error("parse error");
}

bool Parser::isPrintKeyword(TokenType type) const {
    return containsTokenType({
        TokenType::KW_DIKHAO, TokenType::KW_KAADO, TokenType::KW_DAKHO, TokenType::KW_DEKHAO,
        TokenType::KW_BATAVO, TokenType::KW_DAKHAV, TokenType::KW_CHUPINCHU, TokenType::KW_TORO,
        TokenType::KW_KAANIKU, TokenType::KW_DEKHAOW, TokenType::KW_DAKHOI, TokenType::KW_DEKHAU,
        TokenType::KW_DARSHAYA, TokenType::KW_WAATAV, TokenType::KW_DIKHAY, TokenType::KW_WEKHO,
        TokenType::KW_NUNGSI, TokenType::KW_NANGI, TokenType::KW_DADO, TokenType::KW_PRINT
    }, type);
}

bool Parser::isIfKeyword(TokenType type) const {
    return containsTokenType({
        TokenType::KW_AGAR, TokenType::KW_ENDRAL, TokenType::KW_JEKR, TokenType::KW_JODI,
        TokenType::KW_JOR, TokenType::KW_JAR, TokenType::KW_ANTE, TokenType::KW_AGRE,
        TokenType::KW_ENGIL, TokenType::KW_JADI, TokenType::KW_NOHOLE, TokenType::KW_JE,
        TokenType::KW_ZOR, TokenType::KW_YEDI, TokenType::KW_YADI, TokenType::KW_GALAW,
        TokenType::KW_GER, TokenType::KW_AMARE, TokenType::KW_BODO, TokenType::KW_AMA, TokenType::KW_IF
    }, type);
}

bool Parser::isElseKeyword(TokenType type) const {
    return containsTokenType({
        TokenType::KW_WARNA, TokenType::KW_ILLENA, TokenType::KW_MUDMUDKE, TokenType::KW_NAHOLE,
        TokenType::KW_NAHI_TO, TokenType::KW_NAHITAR, TokenType::KW_LEKAPOTE, TokenType::KW_ILLANDRE,
        TokenType::KW_ALLENKIL, TokenType::KW_BAE, TokenType::KW_NAHI_TA, TokenType::KW_NAITAR,
        TokenType::KW_NABHAYE, TokenType::KW_ANYATHA, TokenType::KW_NAHIN_TA, TokenType::KW_NATTE,
        TokenType::KW_NANGOU, TokenType::KW_BAE, TokenType::KW_ELSE
    }, type);
}

bool Parser::isWhileKeyword(TokenType type) const {
    return containsTokenType({
        TokenType::KW_BAARBAAR, TokenType::KW_MARUMURAI, TokenType::KW_MURHKE, TokenType::KW_JABOTOKKHON,
        TokenType::KW_JABTAK, TokenType::KW_PARAT, TokenType::KW_ANDAKA, TokenType::KW_JOTAGJE,
        TokenType::KW_VARETHUM, TokenType::KW_JABAT, TokenType::KW_JABOTLOI, TokenType::KW_JABSAMMA,
        TokenType::KW_YAVAT, TokenType::KW_WAATAV, TokenType::KW_JABSIM, TokenType::KW_CHATLABA,
        TokenType::KW_WHILE
    }, type);
}

bool Parser::isForKeyword(TokenType type) const {
    return type == TokenType::KW_FOR || type == TokenType::KW_BAARBAAR;
}

bool Parser::isFunctionKeyword(TokenType type) const {
    return containsTokenType({
        TokenType::KW_KAAM, TokenType::KW_SEYAL, TokenType::KW_TE, TokenType::KW_KAJ,
        TokenType::KW_PACHHU, TokenType::KW_KHARAY, TokenType::KW_PANI, TokenType::KW_KELU,
        TokenType::KW_CHEYU, TokenType::KW_PHERA, TokenType::KW_GHURAI, TokenType::KW_KARYA,
        TokenType::KW_PHIRAVE, TokenType::KW_TIKIN, TokenType::KW_FUNCTION
    }, type);
}

bool Parser::isReturnKeyword(TokenType type) const {
    return containsTokenType({
        TokenType::KW_WAPAS, TokenType::KW_THIRUMBU, TokenType::KW_JA, TokenType::KW_PHRAO,
        TokenType::KW_TIRIGI, TokenType::KW_HOGI, TokenType::KW_MATTI, TokenType::KW_PHERA,
        TokenType::KW_GHURAI, TokenType::KW_PHARKA, TokenType::KW_PRATYAGA, TokenType::KW_SATO,
        TokenType::KW_CHINGBA, TokenType::KW_RETURN
    }, type);
}

bool Parser::isBreakKeyword(const Token& token) const {
    return toLowerCopy(token.lexeme) == "break";
}

bool Parser::isTrueKeyword(TokenType type) const {
    return containsTokenType({
        TokenType::KW_SACH, TokenType::KW_UNMAI, TokenType::KW_SATYA, TokenType::KW_SACHU,
        TokenType::KW_SACHU, TokenType::KW_KHOTA, TokenType::KW_NIJAM, TokenType::KW_NIJHA,
        TokenType::KW_SATHYAM, TokenType::KW_THAMBAL, TokenType::KW_BEO, TokenType::KW_BAE_HON,
        TokenType::KW_SANO, TokenType::KW_TRUE
    }, type);
}

bool Parser::isFalseKeyword(TokenType type) const {
    return containsTokenType({
        TokenType::KW_JHOOTH, TokenType::KW_POI, TokenType::KW_MITTHA, TokenType::KW_JUTHU,
        TokenType::KW_ABADDHAM, TokenType::KW_SULLU, TokenType::KW_ASATHYAM, TokenType::KW_MAKHAL,
        TokenType::KW_NOA, TokenType::KW_KHOOTA, TokenType::KW_FALSE
    }, type);
}

bool Parser::isNullKeyword(const Token& token) const {
    return toLowerCopy(token.lexeme) == "null";
}

bool Parser::isNotKeyword(TokenType type) const {
    return type == TokenType::OP_NOT || type == TokenType::KW_NOT || type == TokenType::KW_NAHI;
}

bool Parser::isAndKeyword(TokenType type) const {
    return containsTokenType({
        TokenType::KW_AUR, TokenType::KW_AND, TokenType::OP_AND,
        TokenType::KW_MATTRUM, TokenType::KW_EBONG, TokenType::KW_ANE, TokenType::KW_ANI,
        TokenType::KW_MARIYU, TokenType::KW_MATTU, TokenType::KW_UM, TokenType::KW_EBAM,
        TokenType::KW_ARU, TokenType::KW_RA, TokenType::KW_CHA, TokenType::KW_AIN,
        TokenType::KW_AMASUNG, TokenType::KW_AR
    }, type);
}

bool Parser::isOrKeyword(TokenType type) const {
    return containsTokenType({
        TokenType::KW_OR, TokenType::OP_OR, TokenType::KW_YA, TokenType::KW_KE,
        TokenType::KW_KIVA, TokenType::KW_ALLATHU, TokenType::KW_LEDA, TokenType::KW_ATHAVA,
        TokenType::KW_WA, TokenType::KW_YANABA, TokenType::KW_ABUA
    }, type);
}

bool Parser::isFromKeyword(TokenType type) const {
    return type == TokenType::KW_FROM || type == TokenType::KW_SE;
}

bool Parser::isToKeyword(TokenType type) const {
    return type == TokenType::KW_TO || type == TokenType::KW_TAK;
}

bool Parser::isThenKeyword(TokenType type) const {
    return type == TokenType::KW_THEN || type == TokenType::KW_TAB;
}

void Parser::synchronize() {
    while (!isAtEnd()) {
        if (previous().type == TokenType::NEWLINE) {
            return;
        }
        switch (peek().type) {
            case TokenType::KW_IF:
            case TokenType::KW_AGAR:
            case TokenType::KW_DIKHAO:
            case TokenType::KW_LET:
                return;
            default:
                break;
        }
        advance();
    }
}

void Parser::errorAtCurrent(const std::string& message) {
    errorAt(peek(), message);
}

void Parser::errorAt(const Token& token, const std::string& message) {
    had_error = true;
    reportError(ErrorCode::SYNTAX_ERROR, language, token.line);
    (void)message;
}

} // namespace mantra
