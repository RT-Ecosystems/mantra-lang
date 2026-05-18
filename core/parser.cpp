#include "parser.h"
#include "error.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace mantra {

namespace {

std::string toLowerCopy(const std::string& value) {
    std::string result;
    result.reserve(value.size());
    for (char ch : value) {
        result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
    }
    return result;
}

} // namespace

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

const Token& Parser::peekNextNonNewline(size_t start, size_t& out_index) const {
    out_index = start;
    while (out_index < tokens.size()) {
        if (tokens[out_index].type != TokenType::NEWLINE) {
            return tokens[out_index];
        }
        out_index++;
    }
    out_index = tokens.size();
    return tokens.back();
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

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::matchAny(const std::vector<TokenType>& types) {
    for (const auto& type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::skipSeparators() {
    bool saw_newline = false;
    while (match(TokenType::NEWLINE) || match(TokenType::SEMICOLON)) {
        if (previous().type == TokenType::NEWLINE) {
            saw_newline = true;
        }
    }
    return saw_newline;
}

void Parser::skipNewlines() {
    while (match(TokenType::NEWLINE)) {
    }
}

int Parser::peekIndent() const {
    size_t index = current;
    while (index < tokens.size() && tokens[index].type == TokenType::NEWLINE) {
        index++;
    }
    if (index >= tokens.size()) {
        return 0;
    }
    int column = tokens[index].column;
    if (column <= 0) {
        return 0;
    }
    return column - 1;
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>(1, 1);
    parseIndentedStatements(program->statements, 0);
    return program;
}

void Parser::parseIndentedStatements(std::vector<std::unique_ptr<MantraNode>>& out,
                                    int indent_level) {
    bool expect_indent = true;

    while (!isAtEnd()) {
        bool saw_newline = skipSeparators();
        if (expect_indent || saw_newline) {
            if (isAtEnd()) {
                return;
            }
            int indent = peekIndent();
            if (indent < indent_level) {
                return;
            }
            if (indent > indent_level) {
                errorAt(peek(), "अप्रत्याशित इंडेंटेशन मिला");
                synchronizeLine();
                expect_indent = true;
                continue;
            }
        }

        if (isAtEnd()) {
            return;
        }

        auto stmt = declaration(indent_level);
        if (stmt) {
            out.push_back(std::move(stmt));
        }
        expect_indent = false;
    }
}

std::unique_ptr<MantraNode> Parser::declaration(int indent_level) {
    return statement(indent_level);
}

std::unique_ptr<MantraNode> Parser::statement(int indent_level) {
    if (isAtEnd()) {
        return nullptr;
    }

    if (isPrintKeyword(peek().type)) {
        Token keyword = advance();
        return printStatement(keyword);
    }

    if (check(TokenType::KW_RAKHO) || check(TokenType::KW_LET)) {
        Token keyword = advance();
        return variableDeclaration(keyword);
    }

    if (isIfKeyword(peek().type)) {
        Token keyword = advance();
        return ifStatement(keyword, indent_level);
    }

    if (peek().type == TokenType::KW_BAARBAAR) {
        Token keyword = advance();
        if (looksLikeForLoop()) {
            return forStatement(keyword, indent_level);
        }
        return whileStatement(keyword, indent_level);
    }

    if (isWhileKeyword(peek().type)) {
        Token keyword = advance();
        return whileStatement(keyword, indent_level);
    }

    if (isForKeyword(peek().type)) {
        Token keyword = advance();
        return forStatement(keyword, indent_level);
    }

    if (isFunctionKeyword(peek().type)) {
        Token keyword = advance();
        return functionStatement(keyword, indent_level);
    }

    if (isReturnKeyword(peek().type)) {
        Token keyword = advance();
        return returnStatement(keyword);
    }

    if (isBreakToken(peek())) {
        Token keyword = advance();
        return breakStatement(keyword);
    }

    if (match(TokenType::LBRACE)) {
        return braceBlock(previous());
    }

    return expressionStatement();
}

std::unique_ptr<MantraNode> Parser::printStatement(const Token& keyword) {
    if (match(TokenType::OP_ASSIGN)) {
        // optional '=' after print keyword
    }

    if (check(TokenType::NEWLINE) || check(TokenType::SEMICOLON) || check(TokenType::EOF_TOKEN)) {
        errorAt(keyword, "print/dikhao के लिए अभिव्यक्ति अपेक्षित है");
        return std::make_unique<PrintStmtNode>(
            std::make_unique<NullLitNode>(keyword.line, keyword.column),
            keyword.line,
            keyword.column);
    }

    auto value = expression();
    return std::make_unique<PrintStmtNode>(std::move(value), keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::ifStatement(const Token& keyword, int indent_level) {
    auto condition = expression();

    if (isThenKeyword(peek().type)) {
        advance();
    }

    auto then_block = blockOrSingle(keyword, indent_level);
    std::unique_ptr<BlockStmtNode> else_block;

    skipNewlines();
    if (isElseKeyword(peek().type)) {
        Token else_keyword = advance();
        else_block = blockOrSingle(else_keyword, indent_level);
    }

    return std::make_unique<IfStmtNode>(std::move(condition),
                                        std::move(then_block),
                                        std::move(else_block),
                                        keyword.line,
                                        keyword.column);
}

std::unique_ptr<MantraNode> Parser::whileStatement(const Token& keyword, int indent_level) {
    auto condition = expression();
    auto body_block = blockOrSingle(keyword, indent_level);
    return std::make_unique<WhileStmtNode>(std::move(condition),
                                           std::move(body_block),
                                           keyword.line,
                                           keyword.column);
}

std::unique_ptr<MantraNode> Parser::forStatement(const Token& keyword, int indent_level) {
    if (!check(TokenType::IDENTIFIER)) {
        errorAtCurrent("for/बारबार के बाद पहचानकर्ता अपेक्षित है");
        synchronizeLine();
        return nullptr;
    }

    Token name_token = advance();

    if (!match(TokenType::OP_ASSIGN)) {
        errorAtCurrent("for/बारबार में '=' अपेक्षित है");
        synchronizeLine();
        return nullptr;
    }

    auto start_expr = expression();

    bool saw_range_keyword = false;
    if (isFromKeyword(peek().type)) {
        advance();
        saw_range_keyword = true;
    }

    if (!saw_range_keyword && (isToKeyword(peek().type))) {
        advance();
    } else if (!saw_range_keyword && !isToKeyword(peek().type)) {
        errorAtCurrent("for/बारबार में 'se/from' या 'to/tak' अपेक्षित है");
        synchronizeLine();
        return nullptr;
    }

    auto end_expr = expression();

    if (saw_range_keyword) {
        if (!isToKeyword(peek().type)) {
            errorAtCurrent("for/बारबार में 'tak/to' अपेक्षित है");
            synchronizeLine();
            return nullptr;
        }
        advance();
    }

    std::unique_ptr<MantraNode> step_expr;
    if (isStepToken(peek())) {
        advance();
        step_expr = expression();
    }

    auto body_block = blockOrSingle(keyword, indent_level);

    return std::make_unique<ForStmtNode>(name_token.lexeme,
                                         std::move(start_expr),
                                         std::move(end_expr),
                                         std::move(step_expr),
                                         std::move(body_block),
                                         keyword.line,
                                         keyword.column);
}

std::unique_ptr<MantraNode> Parser::functionStatement(const Token& keyword, int indent_level) {
    if (!check(TokenType::IDENTIFIER)) {
        errorAtCurrent("function/kaam के बाद नाम अपेक्षित है");
        synchronizeLine();
        return nullptr;
    }

    Token name_token = advance();

    if (!match(TokenType::LPAREN)) {
        errorAtCurrent("function/kaam के बाद '(' अपेक्षित है");
        synchronizeLine();
        return nullptr;
    }

    std::vector<std::string> params;
    if (!check(TokenType::RPAREN)) {
        do {
            if (!check(TokenType::IDENTIFIER)) {
                errorAtCurrent("parameter नाम अपेक्षित है");
                synchronizeLine();
                break;
            }
            params.push_back(advance().lexeme);
        } while (match(TokenType::COMMA));
    }

    if (!match(TokenType::RPAREN)) {
        errorAtCurrent("')' अपेक्षित है");
        synchronizeLine();
        return nullptr;
    }

    auto body_block = blockOrSingle(keyword, indent_level);

    return std::make_unique<FuncDefNode>(name_token.lexeme,
                                         std::move(params),
                                         std::move(body_block),
                                         keyword.line,
                                         keyword.column);
}

std::unique_ptr<MantraNode> Parser::returnStatement(const Token& keyword) {
    if (check(TokenType::NEWLINE) || check(TokenType::SEMICOLON) ||
        check(TokenType::RBRACE) || check(TokenType::EOF_TOKEN)) {
        return std::make_unique<ReturnStmtNode>(nullptr, keyword.line, keyword.column);
    }

    auto value = expression();
    return std::make_unique<ReturnStmtNode>(std::move(value), keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::breakStatement(const Token& keyword) {
    return std::make_unique<BreakStmtNode>(keyword.line, keyword.column);
}

std::unique_ptr<MantraNode> Parser::variableDeclaration(const Token& keyword) {
    if (!check(TokenType::IDENTIFIER)) {
        errorAtCurrent("rakho/let के बाद नाम अपेक्षित है");
        synchronizeLine();
        return nullptr;
    }

    Token name_token = advance();

    if (!match(TokenType::OP_ASSIGN)) {
        errorAtCurrent("rakho/let में '=' अपेक्षित है");
        synchronizeLine();
        return nullptr;
    }

    auto value = expression();
    return std::make_unique<AssignStmtNode>(name_token.lexeme,
                                            std::move(value),
                                            keyword.line,
                                            keyword.column);
}

std::unique_ptr<MantraNode> Parser::expressionStatement() {
    auto expr = expression();
    if (expr && expr->type == NodeType::ASSIGN_STMT) {
        return expr;
    }
    const Token& token = previous();
    return std::make_unique<ExprStmtNode>(std::move(expr), token.line, token.column);
}

std::unique_ptr<BlockStmtNode> Parser::braceBlock(const Token& owner) {
    auto block_node = std::make_unique<BlockStmtNode>(owner.line, owner.column);

    bool saw_newline = skipSeparators();
    (void)saw_newline;

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        auto stmt = declaration(0);
        if (stmt) {
            block_node->statements.push_back(std::move(stmt));
        }
        skipSeparators();
    }

    if (!match(TokenType::RBRACE)) {
        errorAtCurrent("'}' अपेक्षित है");
    }

    return block_node;
}

std::unique_ptr<BlockStmtNode> Parser::indentBlock(const Token& owner, int indent_level) {
    auto block_node = std::make_unique<BlockStmtNode>(owner.line, owner.column);
    parseIndentedStatements(block_node->statements, indent_level);
    return block_node;
}

std::unique_ptr<BlockStmtNode> Parser::blockOrSingle(const Token& owner, int indent_level) {
    if (match(TokenType::LBRACE)) {
        return braceBlock(owner);
    }

    if (match(TokenType::NEWLINE)) {
        while (match(TokenType::NEWLINE)) {
        }
        int block_indent = peekIndent();
        if (block_indent <= indent_level) {
            errorAt(owner, "इंडेंटेड ब्लॉक अपेक्षित है");
            return std::make_unique<BlockStmtNode>(owner.line, owner.column);
        }
        return indentBlock(owner, block_indent);
    }

    auto block_node = std::make_unique<BlockStmtNode>(owner.line, owner.column);
    auto stmt = declaration(indent_level);
    if (stmt) {
        block_node->statements.push_back(std::move(stmt));
    }
    return block_node;
}

std::unique_ptr<MantraNode> Parser::expression() {
    return assignment();
}

std::unique_ptr<MantraNode> Parser::assignment() {
    auto expr = logicalOr();

    if (match(TokenType::OP_ASSIGN)) {
        const Token& equals = previous();
        auto value = assignment();

        auto* identifier = dynamic_cast<IdentifierNode*>(expr.get());
        if (!identifier) {
            errorAt(equals, "असाइनमेंट के लिए वैध पहचानकर्ता चाहिए");
            return value;
        }

        return std::make_unique<AssignStmtNode>(identifier->name,
                                                std::move(value),
                                                equals.line,
                                                equals.column);
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::logicalOr() {
    auto expr = logicalAnd();

    while (match(TokenType::OP_OR) || isOrKeyword(peek().type)) {
        if (previous().type != TokenType::OP_OR) {
            advance();
        }
        Token op_token = previous();
        auto right = logicalAnd();
        expr = std::make_unique<BinaryExprNode>(TokenType::OP_OR,
                                                std::move(expr),
                                                std::move(right),
                                                op_token.line,
                                                op_token.column);
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::logicalAnd() {
    auto expr = equality();

    while (match(TokenType::OP_AND) || isAndKeyword(peek().type)) {
        if (previous().type != TokenType::OP_AND) {
            advance();
        }
        Token op_token = previous();
        auto right = equality();
        expr = std::make_unique<BinaryExprNode>(TokenType::OP_AND,
                                                std::move(expr),
                                                std::move(right),
                                                op_token.line,
                                                op_token.column);
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::equality() {
    auto expr = comparison();

    while (matchAny({TokenType::OP_EQUAL, TokenType::OP_NOT_EQUAL})) {
        TokenType oper = previous().type;
        auto right = comparison();
        expr = std::make_unique<BinaryExprNode>(oper,
                                                std::move(expr),
                                                std::move(right),
                                                previous().line,
                                                previous().column);
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::comparison() {
    auto expr = term();

    while (matchAny({TokenType::OP_LESS, TokenType::OP_LESS_EQUAL,
                     TokenType::OP_GREATER, TokenType::OP_GREATER_EQUAL})) {
        TokenType oper = previous().type;
        auto right = term();
        expr = std::make_unique<BinaryExprNode>(oper,
                                                std::move(expr),
                                                std::move(right),
                                                previous().line,
                                                previous().column);
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::term() {
    auto expr = factor();

    while (matchAny({TokenType::OP_PLUS, TokenType::OP_MINUS})) {
        TokenType oper = previous().type;
        auto right = factor();
        expr = std::make_unique<BinaryExprNode>(oper,
                                                std::move(expr),
                                                std::move(right),
                                                previous().line,
                                                previous().column);
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::factor() {
    auto expr = unary();

    while (matchAny({TokenType::OP_MULTIPLY, TokenType::OP_DIVIDE, TokenType::OP_MODULO})) {
        TokenType oper = previous().type;
        auto right = unary();
        expr = std::make_unique<BinaryExprNode>(oper,
                                                std::move(expr),
                                                std::move(right),
                                                previous().line,
                                                previous().column);
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::unary() {
    if (matchAny({TokenType::OP_MINUS, TokenType::OP_NOT})) {
        TokenType oper = previous().type;
        auto right = unary();
        return std::make_unique<UnaryExprNode>(oper,
                                               std::move(right),
                                               previous().line,
                                               previous().column);
    }

    if (isNotKeyword(peek().type)) {
        Token tok = advance();
        auto right = unary();
        return std::make_unique<UnaryExprNode>(TokenType::OP_NOT,
                                               std::move(right),
                                               tok.line,
                                               tok.column);
    }

    return call();
}

std::unique_ptr<MantraNode> Parser::call() {
    auto expr = primary();

    while (true) {
        if (match(TokenType::LPAREN)) {
            std::vector<std::unique_ptr<MantraNode>> args;
            if (!check(TokenType::RPAREN)) {
                do {
                    args.push_back(expression());
                } while (match(TokenType::COMMA));
            }

            Token call_token = previous();
            if (!match(TokenType::RPAREN)) {
                errorAtCurrent("')' अपेक्षित है");
                synchronizeLine();
                return expr;
            }

            expr = std::make_unique<CallExprNode>(std::move(expr),
                                                  std::move(args),
                                                  call_token.line,
                                                  call_token.column);
            continue;
        }

        if (match(TokenType::LBRACKET)) {
            auto index_expr = expression();
            Token index_token = previous();
            if (!match(TokenType::RBRACKET)) {
                errorAtCurrent("']' अपेक्षित है");
                synchronizeLine();
                return expr;
            }

            expr = std::make_unique<IndexExprNode>(std::move(expr),
                                                   std::move(index_expr),
                                                   index_token.line,
                                                   index_token.column);
            continue;
        }

        break;
    }

    return expr;
}

std::unique_ptr<MantraNode> Parser::primary() {
    if (matchAny({TokenType::NUMBER_INT, TokenType::NUMBER_FLOAT, TokenType::NUMBER_HEX})) {
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
        return std::make_unique<NumberLitNode>(number_value,
                                               is_integer,
                                               number_token.lexeme,
                                               number_token.line,
                                               number_token.column);
    }

    if (match(TokenType::STRING)) {
        const Token& string_token = previous();
        return std::make_unique<StringLitNode>(string_token.value,
                                               string_token.line,
                                               string_token.column);
    }

    if (isTrueKeyword(peek().type)) {
        Token tok = advance();
        return std::make_unique<BoolLitNode>(true, tok.line, tok.column);
    }

    if (isFalseKeyword(peek().type)) {
        Token tok = advance();
        return std::make_unique<BoolLitNode>(false, tok.line, tok.column);
    }

    if (match(TokenType::LBRACKET)) {
        std::vector<std::unique_ptr<MantraNode>> elements;
        if (!check(TokenType::RBRACKET)) {
            do {
                elements.push_back(expression());
            } while (match(TokenType::COMMA));
        }
        Token array_token = previous();
        if (!match(TokenType::RBRACKET)) {
            errorAtCurrent("']' अपेक्षित है");
        }
        return std::make_unique<ArrayLitNode>(std::move(elements),
                                              array_token.line,
                                              array_token.column);
    }

    if (match(TokenType::IDENTIFIER)) {
        const Token& ident = previous();
        std::string lower = toLowerCopy(ident.lexeme);
        if (lower == "null" || lower == "nil" || lower == "khali") {
            return std::make_unique<NullLitNode>(ident.line, ident.column);
        }
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
    synchronizeLine();
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

bool Parser::isAndKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_AND:
        case TokenType::KW_AUR:
        case TokenType::KW_EBONG:
        case TokenType::KW_ANE:
        case TokenType::KW_ANI:
        case TokenType::KW_MARIYU:
        case TokenType::KW_MATTU:
        case TokenType::KW_UM:
        case TokenType::KW_EBAM:
        case TokenType::KW_ARU:
        case TokenType::KW_VA:
        case TokenType::KW_RA:
        case TokenType::KW_CHA:
        case TokenType::KW_AIN:
        case TokenType::KW_AMASUNG:
        case TokenType::KW_AR:
            return true;
        default:
            return false;
    }
}

bool Parser::isOrKeyword(TokenType type) const {
    switch (type) {
        case TokenType::KW_OR:
        case TokenType::KW_YA:
        case TokenType::KW_KE:
        case TokenType::KW_KIVA:
        case TokenType::KW_LEDA:
        case TokenType::KW_ATHAVA:
        case TokenType::KW_WA:
        case TokenType::KW_YANABA:
        case TokenType::KW_ABUA:
        case TokenType::KW_ALLATHU:
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

bool Parser::looksLikeForLoop() const {
    if (peek().type != TokenType::KW_BAARBAAR) {
        return false;
    }

    size_t idx = current + 1;
    size_t next_index = idx;
    const Token& next = peekNextNonNewline(idx, next_index);
    if (next.type != TokenType::IDENTIFIER) {
        return false;
    }

    size_t after_ident = next_index + 1;
    size_t assign_index = after_ident;
    const Token& after_token = peekNextNonNewline(after_ident, assign_index);
    return after_token.type == TokenType::OP_ASSIGN;
}

bool Parser::isStepToken(const Token& token) const {
    if (token.type != TokenType::IDENTIFIER) {
        return false;
    }
    std::string lower = toLowerCopy(token.lexeme);
    return lower == "step" || lower == "qadam";
}

bool Parser::isBreakToken(const Token& token) const {
    if (token.type != TokenType::IDENTIFIER) {
        return false;
    }
    std::string lower = toLowerCopy(token.lexeme);
    return lower == "break" || lower == "roko" || lower == "rukja";
}

void Parser::synchronizeLine() {
    if (isAtEnd()) {
        return;
    }

    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::NEWLINE || previous().type == TokenType::SEMICOLON) {
            return;
        }

        switch (peek().type) {
            case TokenType::KW_IF:
            case TokenType::KW_WHILE:
            case TokenType::KW_FOR:
            case TokenType::KW_FUNCTION:
            case TokenType::KW_RETURN:
            case TokenType::KW_PRINT:
            case TokenType::KW_DIKHAO:
            case TokenType::KW_BAARBAAR:
            case TokenType::KW_KAAM:
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
