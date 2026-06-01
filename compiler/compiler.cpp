#include "compiler.h"

namespace mantra {

BytecodeCompiler::BytecodeCompiler() = default;

std::shared_ptr<BytecodeProgram> BytecodeCompiler::compile(const ProgramNode& program) {
    program_ = std::make_shared<BytecodeProgram>();
    globals_.clear();
    next_global_index_ = 0;
    
    for (const auto& stmt : program.statements) {
        if (stmt) {
            compileStatement(*stmt);
        }
    }
    
    program_->emit(OpCode::HALT);
    return program_;
}

void BytecodeCompiler::compileStatement(const MantraNode& node) {
    switch (node.type) {
        case NodeType::PRINT_STMT:
            compilePrintStmt(static_cast<const PrintStmtNode&>(node));
            break;
        case NodeType::ASSIGN_STMT:
            compileAssignStmt(static_cast<const AssignStmtNode&>(node));
            break;
        case NodeType::IF_STMT:
            compileIfStmt(static_cast<const IfStmtNode&>(node));
            break;
        case NodeType::WHILE_STMT:
            compileWhileStmt(static_cast<const WhileStmtNode&>(node));
            break;
        case NodeType::FOR_STMT:
            compileForStmt(static_cast<const ForStmtNode&>(node));
            break;
        case NodeType::FUNC_DEF:
            compileFuncDef(static_cast<const FuncDefNode&>(node));
            break;
        case NodeType::RETURN_STMT:
            compileReturnStmt(static_cast<const ReturnStmtNode&>(node));
            break;
        case NodeType::BLOCK_STMT:
            compileBlockStmt(static_cast<const BlockStmtNode&>(node));
            break;
        case NodeType::EXPR_STMT:
            compileExprStmt(static_cast<const ExprStmtNode&>(node));
            break;
        default:
            break;
    }
}

void BytecodeCompiler::compileExpression(const MantraNode& node) {
    switch (node.type) {
        case NodeType::BINARY_EXPR:
            compileBinaryExpr(static_cast<const BinaryExprNode&>(node));
            break;
        case NodeType::UNARY_EXPR:
            compileUnaryExpr(static_cast<const UnaryExprNode&>(node));
            break;
        case NodeType::CALL_EXPR:
            compileCallExpr(static_cast<const CallExprNode&>(node));
            break;
        case NodeType::IDENTIFIER:
            compileIdentifier(static_cast<const IdentifierNode&>(node));
            break;
        case NodeType::NUMBER_LIT:
            compileNumberLit(static_cast<const NumberLitNode&>(node));
            break;
        case NodeType::STRING_LIT:
            compileStringLit(static_cast<const StringLitNode&>(node));
            break;
        case NodeType::BOOL_LIT:
            compileBoolLit(static_cast<const BoolLitNode&>(node));
            break;
        case NodeType::NULL_LIT:
            compileNullLit(static_cast<const NullLitNode&>(node));
            break;
        default:
            break;
    }
}

void BytecodeCompiler::compilePrintStmt(const PrintStmtNode& node) {
    if (node.expression) {
        compileExpression(*node.expression);
        program_->emit(OpCode::PRINT);
    }
}

void BytecodeCompiler::compileAssignStmt(const AssignStmtNode& node) {
    if (!node.value) {
        return;
    }
    
    compileExpression(*node.value);
    
    if (node.is_declaration) {
        // New variable
        int index = next_global_index_++;
        globals_[node.name] = index;
        program_->emit(OpCode::DEFINE_GLOBAL);
    } else {
        // Assignment to existing variable
        auto it = globals_.find(node.name);
        if (it != globals_.end()) {
            program_->emit(OpCode::SET_GLOBAL, it->second);
        }
    }
}

void BytecodeCompiler::compileIfStmt(const IfStmtNode& node) {
    // Compile condition
    if (node.condition) {
        compileExpression(*node.condition);
    }
    
    // Emit jump instruction (to be patched)
    std::size_t then_jump = program_->emitJump(OpCode::JUMP_IF_FALSE);
    
    // Pop the condition value
    program_->emit(OpCode::POP);
    
    // Compile then branch
    if (node.then_branch) {
        compileBlockStmt(*node.then_branch);
    }
    
    // Emit else jump (to be patched)
    std::size_t else_jump = program_->emitJump(OpCode::JUMP);
    
    // Patch then_jump to jump here
    program_->patchJump(then_jump);
    program_->emit(OpCode::POP); // Pop condition
    
    // Compile else branch
    if (node.else_branch) {
        compileBlockStmt(*node.else_branch);
    }
    
    // Patch else_jump to jump here
    program_->patchJump(else_jump);
}

void BytecodeCompiler::compileWhileStmt(const WhileStmtNode& node) {
    std::size_t loop_start = program_->currentOffset();
    
    // Compile condition
    if (node.condition) {
        compileExpression(*node.condition);
    }
    
    // Emit jump if false
    std::size_t exit_jump = program_->emitJump(OpCode::JUMP_IF_FALSE);
    
    // Pop condition
    program_->emit(OpCode::POP);
    
    // Compile body
    if (node.body) {
        compileBlockStmt(*node.body);
    }
    
    // Emit loop back
    program_->emit(OpCode::LOOP, loop_start);
    
    // Patch exit jump
    program_->patchJump(exit_jump);
    program_->emit(OpCode::POP); // Pop condition
}

void BytecodeCompiler::compileForStmt(const ForStmtNode& node) {
    // Compile start value
    if (node.start) {
        compileExpression(*node.start);
    }
    
    // Define loop variable
    int index = next_global_index_++;
    globals_[node.variable] = index;
    program_->emit(OpCode::DEFINE_GLOBAL);
    
    std::size_t loop_start = program_->currentOffset();
    
    // Compile condition (variable <= end)
    program_->emit(OpCode::GET_GLOBAL, index);
    if (node.end) {
        compileExpression(*node.end);
    }
    program_->emit(OpCode::LESS_EQUAL);
    
    std::size_t exit_jump = program_->emitJump(OpCode::JUMP_IF_FALSE);
    program_->emit(OpCode::POP);
    
    // Compile body
    if (node.body) {
        compileBlockStmt(*node.body);
    }
    
    // Increment variable
    program_->emit(OpCode::GET_GLOBAL, index);
    if (node.step) {
        compileExpression(*node.step);
    } else {
        program_->emit(OpCode::PUSH_CONST, program_->addConstant(Value::number(1)));
    }
    program_->emit(OpCode::ADD);
    program_->emit(OpCode::SET_GLOBAL, index);
    program_->emit(OpCode::POP);
    
    // Loop back
    program_->emit(OpCode::LOOP, loop_start);
    
    // Patch exit
    program_->patchJump(exit_jump);
    program_->emit(OpCode::POP);
}

void BytecodeCompiler::compileFuncDef(const FuncDefNode& node) {
    // For now, skip function definitions as they require more complex compilation
    // This is a placeholder for function definitions
}

void BytecodeCompiler::compileReturnStmt(const ReturnStmtNode& node) {
    if (node.value) {
        compileExpression(*node.value);
    } else {
        program_->emit(OpCode::PUSH_CONST, program_->addConstant(Value::null()));
    }
    program_->emit(OpCode::RETURN);
}

void BytecodeCompiler::compileBlockStmt(const BlockStmtNode& node) {
    for (const auto& stmt : node.statements) {
        if (stmt) {
            compileStatement(*stmt);
        }
    }
}

void BytecodeCompiler::compileExprStmt(const ExprStmtNode& node) {
    if (node.expression) {
        compileExpression(*node.expression);
        // Pop the result if it's not used
        program_->emit(OpCode::POP);
    }
}

void BytecodeCompiler::compileBinaryExpr(const BinaryExprNode& node) {
    if (!node.left || !node.right) {
        return;
    }
    
    // Compile left operand
    compileExpression(*node.left);
    
    // Compile right operand
    compileExpression(*node.right);
    
    // Emit operation
    switch (node.op) {
        case TokenType::OP_PLUS:
            program_->emit(OpCode::ADD);
            break;
        case TokenType::OP_MINUS:
            program_->emit(OpCode::SUB);
            break;
        case TokenType::OP_MULTIPLY:
            program_->emit(OpCode::MUL);
            break;
        case TokenType::OP_DIVIDE:
            program_->emit(OpCode::DIV);
            break;
        case TokenType::OP_EQUAL:
            program_->emit(OpCode::EQUAL);
            break;
        case TokenType::OP_NOT_EQUAL:
            program_->emit(OpCode::NOT_EQUAL);
            break;
        case TokenType::OP_GREATER:
            program_->emit(OpCode::GREATER);
            break;
        case TokenType::OP_GREATER_EQUAL:
            program_->emit(OpCode::GREATER_EQUAL);
            break;
        case TokenType::OP_LESS:
            program_->emit(OpCode::LESS);
            break;
        case TokenType::OP_LESS_EQUAL:
            program_->emit(OpCode::LESS_EQUAL);
            break;
        default:
            break;
    }
}

void BytecodeCompiler::compileUnaryExpr(const UnaryExprNode& node) {
    if (!node.operand) {
        return;
    }
    
    compileExpression(*node.operand);
    
    if (node.op == TokenType::OP_NOT) {
        program_->emit(OpCode::NOT);
    } else if (node.op == TokenType::OP_MINUS) {
        program_->emit(OpCode::NEGATE);
    }
}

void BytecodeCompiler::compileCallExpr(const CallExprNode& node) {
    // For now, skip function calls as they require more complex compilation
    // This is a placeholder for function calls
}

void BytecodeCompiler::compileIdentifier(const IdentifierNode& node) {
    auto it = globals_.find(node.name);
    if (it != globals_.end()) {
        program_->emit(OpCode::GET_GLOBAL, it->second);
    }
}

void BytecodeCompiler::compileNumberLit(const NumberLitNode& node) {
    std::size_t index = program_->addConstant(Value::number(node.value));
    program_->emit(OpCode::PUSH_CONST, index);
}

void BytecodeCompiler::compileStringLit(const StringLitNode& node) {
    std::size_t index = program_->addConstant(Value::string(node.value));
    program_->emit(OpCode::PUSH_CONST, index);
}

void BytecodeCompiler::compileBoolLit(const BoolLitNode& node) {
    program_->emit(node.value ? OpCode::TRUE : OpCode::FALSE);
}

void BytecodeCompiler::compileNullLit(const NullLitNode& node) {
    std::size_t index = program_->addConstant(Value::null());
    program_->emit(OpCode::PUSH_CONST, index);
}

} // namespace mantra

