#pragma once

#include "../core/ast.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mantra {

class SemanticAnalyzer {
public:
    SemanticAnalyzer();

    bool analyze(const ProgramNode& program);
    bool hasErrors() const { return had_error; }

private:
    enum class SymbolKind {
        Variable,
        Function
    };

    struct FunctionSignature {
        size_t min_arity = 0;
        size_t max_arity = 0;
    };

    using Scope = std::unordered_map<std::string, SymbolKind>;

    std::vector<Scope> scopes;
    std::unordered_map<std::string, FunctionSignature> functions;
    std::unordered_map<std::string, FunctionSignature> builtins;
    std::unordered_set<std::string> constants;
    int loop_depth;
    int function_depth;
    bool had_error;

    void pushScope();
    void popScope();

    bool declare(const std::string& name, SymbolKind kind, const MantraNode& node, const std::string& message);
    bool isDefined(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    const FunctionSignature* lookupFunction(const std::string& name) const;
    void registerFunction(const std::string& name, size_t min_arity, size_t max_arity);
    void registerBuiltin(const std::string& name, size_t min_arity, size_t max_arity);
    void registerConstant(const std::string& name);

    void analyzeStatements(const std::vector<std::unique_ptr<MantraNode>>& statements);
    void analyzeNode(const MantraNode& node);
    void analyzeBlock(const BlockStmtNode& block);
    void analyzeExpression(const MantraNode& node);

    void report(const MantraNode& node, const std::string& message);
    void seedBuiltins();
};

} // namespace mantra
