

// === FIXED FUNCTIONS ADDED BY GROK ===
std::unique_ptr<Expr> Parser::parseAssignment() {
    auto expr = parseLogicalOr();

    if (match(TokenType::EQUAL)) {
        Token equals = previous();
        auto value = parseAssignment();

        if (auto* var = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(var->name, std::move(value));
        } else {
            error(equals, "Invalid assignment target. Only variables can be assigned.");
            return nullptr;
        }
    }
    return expr;
}

std::unique_ptr<Expr> Parser::parseEquality() {
    auto expr = parseComparison();

    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL)) {
        Token op = previous();
        auto right = parseComparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}
