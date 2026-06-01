#pragma once

#include "../core/ast.h"
#include "mantra/vm/bytecode.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace mantra {

class BytecodeCompiler {
public:
    BytecodeCompiler();

    std::shared_ptr<BytecodeProgram> compile(const ProgramNode& program);

private:
    std::shared_ptr<BytecodeProgram> program_;
    std::unordered_map<std::string, int> globals_;
    int next_global_index_ = 0;
    
    void compileStatement(const MantraNode& node);
    void compileExpression(const MantraNode& node);
    
    void compilePrintStmt(const PrintStmtNode& node);
    void compileAssignStmt(const AssignStmtNode& node);
    void compileIfStmt(const IfStmtNode& node);
    void compileWhileStmt(const WhileStmtNode& node);
    void compileForStmt(const ForStmtNode& node);
    void compileFuncDef(const FuncDefNode& node);
    void compileReturnStmt(const ReturnStmtNode& node);
    void compileBlockStmt(const BlockStmtNode& node);
    void compileExprStmt(const ExprStmtNode& node);
    
    void compileBinaryExpr(const BinaryExprNode& node);
    void compileUnaryExpr(const UnaryExprNode& node);
    void compileCallExpr(const CallExprNode& node);
    void compileIdentifier(const IdentifierNode& node);
    void compileNumberLit(const NumberLitNode& node);
    void compileStringLit(const StringLitNode& node);
    void compileBoolLit(const BoolLitNode& node);
    void compileNullLit(const NullLitNode& node);
};

} // namespace mantra
