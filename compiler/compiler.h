#pragma once

#include "../bytecode/chunk.h"
#include "../core/ast.h"

#include <memory>
#include <string>
#include <vector>

namespace mantra {

class BytecodeCompiler {
public:
    BytecodeCompiler();

    std::shared_ptr<BytecodeChunk> compile(const ProgramNode& program);

private:
    struct LoopContext {
        std::size_t loop_start = 0;
        std::size_t continue_target = 0;
        bool has_continue_target = false;
        std::vector<std::size_t> break_jumps;
        std::vector<std::size_t> continue_jumps;
    };

    std::shared_ptr<BytecodeChunk> chunk;
    std::vector<LoopContext> loops;
    std::size_t temp_counter;

    void compileStatements(const std::vector<std::unique_ptr<MantraNode>>& statements);
    void compileNode(const MantraNode& node);
    void compileStatement(const MantraNode& node);
    void compileExpression(const MantraNode& node);

    void compileIf(const IfStmtNode& node);
    void compileWhile(const WhileStmtNode& node);
    void compileFor(const ForStmtNode& node);
    void compileFunction(const FuncDefNode& node);
    void compileLogicalAnd(const BinaryExprNode& node);
    void compileLogicalOr(const BinaryExprNode& node);

    void emitConstant(const MantraValue& value, int line, int column);
    void emitName(OpCode op, const std::string& name, int line, int column);
    void emitJumpPlaceholder(OpCode op, int line, int column);
    void patchJump(std::size_t instruction_index);
    std::size_t makeTempName();
    std::string tempName(const std::string& prefix);
    void emitBooleanize(int line, int column);

    void pushLoop(std::size_t loop_start, bool has_continue_target = false, std::size_t continue_target = 0);
    LoopContext& currentLoop();
    void popLoop();
};

} // namespace mantra
