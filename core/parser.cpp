#include "parser.h"
#include "error.h"
#include <cstdlib>
#include <sstream>

namespace mantra {

Parser::Parser(const std::vector<Token>& tokens_input)
    : tokens(tokens_input), current(0), had_error(false) {}

const Token& Parser::peek() const {
    if (current >= tokens.size()) {
        return tokens.back();
    }
    return tokens[current];
}

const Token& Parser::previous() const {
    if (current == 0) {
        return tokens.front();
    }
    return tokens[current - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EOF_TOKEN;
}

const Token& Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (const auto& type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::skipNewlines() {
    while (match(TokenType::NEWLINE) || match(TokenType::SEMICOLON)) {
        // consume
    }
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>(1, 1);
    skipNewlines();
    while (!isAtEnd()) {
        auto stmt = declaration();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
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
    if (isAtEnd()) {
        return nullptr;
    }

    if (isPrintKeyword(peek().type)) {
        advance();
        return printStatement();
    }

    if (isIfKeyword(peek().type)) {
        advance();
        return ifStatement();
    }

    if (isWhileKeyword(peek().type)) {
        advance();
        return whileStatement();
    }

    if (isForKeyword(peek().type)) {
        advance();
        return forStatement();
    }

    if (isFunctionKeyword(peek().type)) {
        advance();
        return functionStatement();
    }

    if (isReturnKeyword(peek().type)) {
        advance();
        return returnStatement();
    }

    if (match(TokenType::LBRACE)) {
        return block();
    }

    return expressionStatement();
}

std::unique_ptr<MantraNode> Parser::printStatement() {
    const Token& keyword = previous();
    auto value = expression();
    return std::make_unique<PrintStmtNode>(std::move(value), keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::ifStatement() {
    const Token& keyword = previous();
    auto condition = expression();
    if (isThenKeyword(peek().type)) {
        advance();
    }

    auto then_block = blockOrSingle();
    std::unique_ptr<BlockStmtNode> else_block;

    if (isElseKeyword(peek().type)) {
        advance();
        else_block = blockOrSingle();
    }

    return std::make_unique<IfStmtNode>(std::move(condition), std::move(then_block),
                                   std::move(else_block), keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::whileStatement() {
    const Token& keyword = previous();
    auto condition = expression();
    auto body_block = blockOrSingle();
    return std::make_unique<WhileStmtNode>(std::move(condition), std::move(body_block),
                                       keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::forStatement() {
    const Token& keyword = previous();

    if (!check(TokenType::IDENTIFIER)) {
        errorAtCurrent("for/बारबार के बाद पहचानकर्ता अपेक्षित है");
        synchronize();
        return nullptr;
    }
    Token name_token = advance();

    if (!match(TokenType::OP_ASSIGN)) {
        errorAtCurrent("for/बारबार में '=' अपेक्षित है");
        synchronize();
        return nullptr;
    }

    auto start_expr = expression();

    if (!isFromKeyword(peek().type)) {
        errorAtCurrent("for/बारबार में 'se/from' अपेक्षित है");
        synchronize();
        return nullptr;
    }
    advance();

    auto end_expr = expression();

    if (!isToKeyword(peek().type)) {
        errorAtCurrent("for/बारबार में 'tak/to' अपेक्षित है");
        synchronize();
        return nullptr;
    }
    advance();

    auto body_block = blockOrSingle();

    return std::make_unique<ForStmtNode>(name_token.lexeme, std::move(start_expr),
                                     std::move(end_expr), std::move(body_block),
                                     keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::functionStatement() {
    const Token& keyword = previous();

    if (!check(TokenType::IDENTIFIER)) {
        errorAtCurrent("function/kaam के बाद नाम अपेक्षित है");
        synchronize();
        return nullptr;
    }

    Token name_token = advance();

    if (!match(TokenType::LPAREN)) {
        errorAtCurrent("function/kaam के बाद '(' अपेक्षित है");
        synchronize();
        return nullptr;
    }

    std::vector<std::string> params;
    if (!check(TokenType::RPAREN)) {
        do {
            if (!check(TokenType::IDENTIFIER)) {
                errorAtCurrent("parameter नाम अपेक्षित है");
                synchronize();
                break;
            }
            params.push_back(advance().lexeme);
        } while (match(TokenType::COMMA));
    }

    if (!match(TokenType::RPAREN)) {
        errorAtCurrent("')' अपेक्षित है");
        synchronize();
        return nullptr;
    }

    auto body_block = blockOrSingle();

    return std::make_unique<FuncDefNode>(name_token.lexeme, params,
                                          std::move(body_block),
                                          keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::returnStatement() {
    const Token& keyword = previous();

    if (check(TokenType::NEWLINE) || check(TokenType::SEMICOLON) ||
        check(TokenType::RBRACE) || check(TokenType::EOF_TOKEN)) {
        return std::make_unique<ReturnStmtNode>(nullptr, keyword.line, keyword.column);
    }

    auto value = expression();
    return std::make_unique<ReturnStmtNode>(std::move(value), keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::expressionStatement() {
    auto expr = expression();
    const Token& token = previous();
    return std::make_unique<ExprStmtNode>(std::move(expr), token.line, token.column);
}

std::unique_ptr<BlockStmtNode> Parser::block() {
    const Token& start = previous();
    auto block_node = std::make_unique<BlockStmtNode>(start.line, start.column);

    skipNewlines();

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        auto stmt = declaration();
        if (stmt) {
            block_node->statements.push_back(std::move(stmt));
        }
        skipNewlines();
    }

    if (!match(TokenType::RBRACE)) {
        errorAtCurrent("'}' अपेक्षित है");
    }

    return block_node;
}

std::unique_ptr<BlockStmtNode> Parser::blockOrSingle() {
    if (match(TokenType::LBRACE)) {
        return block();
    }

    auto block_node = std::make_unique<BlockStmtNode>(peek().line, peek().column);
    auto stmt = statement();
    if (stmt) {
        block_node->statements.push_back(std::move(stmt));
    }
    return block_node;
}

std::unique_ptr<MantraNode> Parser::assignment() {
    auto expr = equality();

    if (match(TokenType::OP_ASSIGN)) {
        const Token& equals = previous();
        auto value = assignment();

        auto* identifier = dynamic_cast<IdentifierNode*>(expr.get());
        if (!identifier) {
            errorAt(equals, "असाइनमेंट के लिए वैध पहचानकर्ता चाहिए");
            return value;
        }

        return std::make_unique<AssignStmtNode>(identifier->name, std::move(value),
                                            equals.line, equals.column);
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::expression() {
    return assignment();
}

std::unique_ptr<MantraNode> Parser::equality() {
    auto expr = comparison();

    while (match({TokenType::OP_EQUAL, TokenType::OP_NOT_EQUAL})) {
        TokenType oper = previous().type;
        auto right = comparison();
        expr = std::make_unique<BinaryExprNode>(oper, std::move(expr), std::move(right),
                                            previous().line, previous().column);
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::comparison() {
    auto expr = term();

    while (match({TokenType::OP_LESS, TokenType::OP_LESS_EQUAL,
                  TokenType::OP_GREATER, TokenType::OP_GREATER_EQUAL})) {
        TokenType oper = previous().type;
        auto right = term();
        expr = std::make_unique<BinaryExprNode>(oper, std::move(expr), std::move(right),
                                            previous().line, previous().column);
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::term() {
    auto expr = factor();

    while (match({TokenType::OP_PLUS, TokenType::OP_MINUS})) {
        TokenType oper = previous().type;
        auto right = factor();
        expr = std::make_unique<BinaryExprNode>(oper, std::move(expr), std::move(right),
                                            previous().line, previous().column);
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::factor() {
    auto expr = unary();

    while (match({TokenType::OP_MULTIPLY, TokenType::OP_DIVIDE})) {
        TokenType oper = previous().type;
        auto right = unary();
        expr = std::make_unique<BinaryExprNode>(oper, std::move(expr), std::move(right),
                                            previous().line, previous().column);
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::unary() {
    if (match({TokenType::OP_MINUS, TokenType::OP_NOT})) {
        TokenType oper = previous().type;
        auto right = unary();
        return std::make_unique<UnaryExprNode>(oper, std::move(right),
                                           previous().line, previous().column);
    }

    if (isNotKeyword(peek().type)) {
        advance();
        TokenType oper = TokenType::OP_NOT;
        auto right = unary();
        return std::make_unique<UnaryExprNode>(oper, std::move(right),
                                           previous().line, previous().column);
    }

    return call();
}

std::unique_ptr<MantraNode> Parser::call() {
    auto expr = primary();

    while (match(TokenType::LPAREN)) {
        std::vector<std::unique_ptr<MantraNode>> args;
        if (!check(TokenType::RPAREN)) {
            do {
                args.push_back(expression());
            } while (match(TokenType::COMMA));
        }

        Token call_token = previous();
        if (!match(TokenType::RPAREN)) {
            errorAtCurrent("')' अपेक्षित है");
            synchronize();
            return expr;
        }

        expr = std::make_unique<CallExprNode>(std::move(expr), std::move(args),
                                          call_token.line, call_token.column);
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::primary() {
    if (match({TokenType::NUMBER_INT, TokenType::NUMBER_FLOAT, TokenType::NUMBER_HEX})) {
        const Token& number_token = previous();
        bool is_integer = number_token.type != TokenType::NUMBER_FLOAT;
        double number_value = 0.0;
        try {
            if (number_token.type == TokenType::NUMBER_HEX) {
                number_value = static_cast<double>(std::stoll(number_token.lexeme, nullptr, 16));
            } else {
                number_value = std::stod(number_token.lexeme);
            }
        } catch (const std::exception&) {
            errorAt(number_token, "संख्या को पढ़ा नहीं जा सका");
        }
        return std::make_unique<NumberLitNode>(number_value, is_integer, number_token.lexeme,
                                            number_token.line, number_token.column);
    }

    if (match(TokenType::STRING)) {
        const Token& string_token = previous();
        return std::make_unique<StringLitNode>(string_token.value,
                                            string_token.line, string_token.column);
    }

    if (isTrueKeyword(peek().type)) {
        Token tok = advance();
        return std::make_unique<BoolLitNode>(true, tok.line, tok.column);
    }

    if (isFalseKeyword(peek().type)) {
        Token tok = advance();
        return std::make_unique<BoolLitNode>(false, tok.line, tok.column);
    }

    if (match(TokenType::IDENTIFIER)) {
        const Token& ident = previous();
        return std::make_unique<IdentifierNode>(ident.lexeme, ident.line, ident.column);
    }

    if (match(TokenType::LPAREN)) {
        auto expr = expression();
        if (!match(TokenType::RPAREN)) {
            errorAtCurrent("')' अपेक्षित है");
        }
        return expr;
    }

    errorAtCurrent("अप्रत्याशित टोकन मिला");
    synchronize();
    return std::make_unique<IdentifierNode>("<error>", peek().line, peek().column);
}

bool Parser::isPrintKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_PRINT:
        case TokenType::KW_DIKHAO:
        case TokenType::KW_DAKHO:
        case TokenType::KW_DEKHAO:
        case TokenType::KW_BATAVO:
        case TokenType::KW_DAKHAV:
        case TokenType::KW_CHUPINCHU:
        case TokenType::KW_TORO:
        case TokenType::KW_KAANIKU:
        case TokenType::KW_DARSHAYA:
        case TokenType::KW_WAATAV:
        case TokenType::KW_DIKHAY:
        case TokenType::KW_WEKHO:
        case TokenType::KW_NUNGSI:
        case TokenType::KW_NANGI:
        case TokenType::KW_DADO:
        case TokenType::KW_DEKHAOW:
        case TokenType::KW_DAKHOI:
        case TokenType::KW_DEKHAU:
        case TokenType::KW_KAADO:
            return true;
        default:
            return false;
    }
}

bool Parser::isIfKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_IF:
        case TokenType::KW_AGAR:
        case TokenType::KW_JEKR:
        case TokenType::KW_JODI:
        case TokenType::KW_JOR:
        case TokenType::KW_JAR:
        case TokenType::KW_ANTE:
        case TokenType::KW_AGRE:
        case TokenType::KW_ENGIL:
        case TokenType::KW_JADI:
        case TokenType::KW_NOHOLE:
        case TokenType::KW_JE:
        case TokenType::KW_ZOR:
        case TokenType::KW_YEDI:
        case TokenType::KW_YADI:
        case TokenType::KW_GALAW:
        case TokenType::KW_GER:
        case TokenType::KW_AMARE:
        case TokenType::KW_BODO:
        case TokenType::KW_AMA:
        case TokenType::KW_ENDRAL:
            return true;
        default:
            return false;
    }
}

bool Parser::isElseKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_ELSE:
        case TokenType::KW_WARNA:
        case TokenType::KW_MUDMUDKE:
        case TokenType::KW_NAHOLE:
        case TokenType::KW_NAHI_TO:
        case TokenType::KW_NAHITAR:
        case TokenType::KW_LEKAPOTE:
        case TokenType::KW_ILLANDRE:
        case TokenType::KW_ALLENKIL:
        case TokenType::KW_NABHAYE:
        case TokenType::KW_ANYATHA:
        case TokenType::KW_NAHIN_TA:
        case TokenType::KW_NAHI_TA:
        case TokenType::KW_NAITAR:
        case TokenType::KW_NATTE:
        case TokenType::KW_NANGOU:
        case TokenType::KW_BAE:
        case TokenType::KW_ILLENA:
            return true;
        default:
            return false;
    }
}

bool Parser::isWhileKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_WHILE:
        case TokenType::KW_BAARBAAR:
        case TokenType::KW_MARUMURAI:
        case TokenType::KW_MURHKE:
        case TokenType::KW_JABOTOKKHON:
        case TokenType::KW_JABTAK:
        case TokenType::KW_PARAT:
        case TokenType::KW_ANDAKA:
        case TokenType::KW_JOTAGJE:
        case TokenType::KW_VARETHUM:
        case TokenType::KW_JABAT:
        case TokenType::KW_JABOTLOI:
        case TokenType::KW_JABSAMMA:
        case TokenType::KW_YAVAT:
        case TokenType::KW_CHATLABA:
        case TokenType::KW_JABSIM:
        case TokenType::KW_JABTO:
            return true;
        default:
            return false;
    }
}

bool Parser::isForKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_FOR:
        case TokenType::KW_BAARBAAR:
            return true;
        default:
            return false;
    }
}

bool Parser::isFunctionKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_FUNCTION:
        case TokenType::KW_KAAM:
        case TokenType::KW_KAJ:
        case TokenType::KW_SEYAL:
        case TokenType::KW_PACHHU:
        case TokenType::KW_KHARAY:
        case TokenType::KW_PANI:
        case TokenType::KW_KELU:
        case TokenType::KW_CHEYU:
        case TokenType::KW_KARYA:
        case TokenType::KW_THABAK:
        case TokenType::KW_PHIRAVE:
        case TokenType::KW_TIKIN:
        case TokenType::KW_TE:
            return true;
        default:
            return false;
    }
}

bool Parser::isReturnKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_RETURN:
        case TokenType::KW_WAPAS:
        case TokenType::KW_THIRUMBU:
        case TokenType::KW_JA:
        case TokenType::KW_PHRAO:
        case TokenType::KW_PACHHU:
        case TokenType::KW_MATTI:
        case TokenType::KW_TIRIGI:
        case TokenType::KW_HOGI:
        case TokenType::KW_PHERA:
        case TokenType::KW_GHURAI:
        case TokenType::KW_PHARKA:
        case TokenType::KW_PRATYAGA:
        case TokenType::KW_PAROT:
        case TokenType::KW_CHINGBA:
        case TokenType::KW_PHIRAVE:
        case TokenType::KW_HON:
            return true;
        default:
            return false;
    }
}

bool Parser::isTrueKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_TRUE:
        case TokenType::KW_SACH:
        case TokenType::KW_UNMAI:
        case TokenType::KW_SATYA:
        case TokenType::KW_SACHU:
        case TokenType::KW_KHARAY:
        case TokenType::KW_NIJAM:
        case TokenType::KW_NIJHA:
        case TokenType::KW_SATHYAM:
        case TokenType::KW_SATA:
        case TokenType::KW_SANO:
        case TokenType::KW_THAMBAL:
        case TokenType::KW_SATO:
        case TokenType::KW_BAE_HON:
            return true;
        default:
            return false;
    }
}

bool Parser::isFalseKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_FALSE:
        case TokenType::KW_JHOOTH:
        case TokenType::KW_POI:
        case TokenType::KW_MITTHA:
        case TokenType::KW_JUTHU:
        case TokenType::KW_KHOTA:
        case TokenType::KW_ABADDHAM:
        case TokenType::KW_SULLU:
        case TokenType::KW_ASATHYAM:
        case TokenType::KW_MITHYA:
        case TokenType::KW_MITHA:
        case TokenType::KW_JHUTO:
        case TokenType::KW_KHOOTA:
        case TokenType::KW_MAKHAL:
        case TokenType::KW_NOA:
            return true;
        default:
            return false;
    }
}

bool Parser::isNotKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_NOT:
        case TokenType::KW_NAHI:
            return true;
        default:
            return false;
    }
}

bool Parser::isFromKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_FROM:
        case TokenType::KW_SE:
            return true;
        default:
            return false;
    }
}

bool Parser::isToKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_TO:
        case TokenType::KW_TAK:
            return true;
        default:
            return false;
    }
}

bool Parser::isThenKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_THEN:
        case TokenType::KW_TAB:
            return true;
        default:
            return false;
    }
}

void Parser::synchronize() {
    if (isAtEnd()) {
        return;
    }

    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON || previous().type == TokenType::NEWLINE) {
            return;
        }

        switch (peek().type) {
            case TokenType::KW_IF:
            case TokenType::KW_WHILE:
            case TokenType::KW_FOR:
            case TokenType::KW_FUNCTION:
            case TokenType::KW_RETURN:
            case TokenType::KW_PRINT:
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
    ErrorHandler::printError(ErrorType::SYNTAX_ERROR, message, token.line, token.column);
}

} // namespace mantra
