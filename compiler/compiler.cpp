#include "compiler.h"

#include <cmath>
#include <stdexcept>

namespace mantra {

BytecodeCompiler::BytecodeCompiler()
    : chunk(std::make_shared<BytecodeChunk>("main")), temp_counter(0) {}

std::shared_ptr<BytecodeChunk> BytecodeCompiler::compile(const ProgramNode& program) {
    chunk = std::make_shared<BytecodeChunk>("main");
    loops.clear();
    temp_counter = 0;
    compileStatements(program.statements);
    chunk->emit(OpCode::Null);
    chunk->emit(OpCode::Return);
    return chunk;
}

void BytecodeCompiler::compileStatements(const std::vector<std::unique_ptr<MantraNode>>& statements) {
    for (const auto& stmt : statements) {
        if (stmt) {
            compileNode(*stmt);
        }
    }
}

void BytecodeCompiler::compileNode(const MantraNode& node) {
    switch (node.type) {
        case NodeType::PROGRAM:
            compileStatements(static_cast<const ProgramNode&>(node).statements);
            break;
        case NodeType::BLOCK_STMT:
            compileStatement(node);
            break;
        default:
            compileStatement(node);
            break;
    }
}

void BytecodeCompiler::compileStatement(const MantraNode& node) {
    switch (node.type) {
        case NodeType::PRINT_STMT: {
            const auto& print_node = static_cast<const PrintStmtNode&>(node);
            if (print_node.expression) {
                compileExpression(*print_node.expression);
            } else {
                chunk->emit(OpCode::Null, 0, node.line, node.column);
            }
            chunk->emit(OpCode::Print, 0, node.line, node.column);
            break;
        }
        case NodeType::ASSIGN_STMT: {
            const auto& assign = static_cast<const AssignStmtNode&>(node);
            if (assign.value) {
                compileExpression(*assign.value);
            } else {
                chunk->emit(OpCode::Null, 0, node.line, node.column);
            }
            emitName(assign.is_declaration ? OpCode::DefineName : OpCode::SetName,
                     assign.name, node.line, node.column);
            break;
        }
        case NodeType::EXPR_STMT: {
            const auto& expr = static_cast<const ExprStmtNode&>(node);
            if (expr.expression) {
                compileExpression(*expr.expression);
                chunk->emit(OpCode::Pop, 0, node.line, node.column);
            }
            break;
        }
        case NodeType::BLOCK_STMT: {
            const auto& block = static_cast<const BlockStmtNode&>(node);
            chunk->emit(OpCode::PushScope, 0, node.line, node.column);
            compileStatements(block.statements);
            chunk->emit(OpCode::PopScope, 0, node.line, node.column);
            break;
        }
        case NodeType::IF_STMT:
            compileIf(static_cast<const IfStmtNode&>(node));
            break;
        case NodeType::WHILE_STMT:
            compileWhile(static_cast<const WhileStmtNode&>(node));
            break;
        case NodeType::FOR_STMT:
            compileFor(static_cast<const ForStmtNode&>(node));
            break;
        case NodeType::FUNC_DEF:
            compileFunction(static_cast<const FuncDefNode&>(node));
            break;
        case NodeType::RETURN_STMT: {
            const auto& ret = static_cast<const ReturnStmtNode&>(node);
            if (ret.value) {
                compileExpression(*ret.value);
            } else {
                chunk->emit(OpCode::Null, 0, node.line, node.column);
            }
            chunk->emit(OpCode::Return, 0, node.line, node.column);
            break;
        }
        case NodeType::BREAK_STMT: {
            if (loops.empty()) {
                throw std::runtime_error("break outside loop");
            }
            auto& loop = currentLoop();
            auto jump = chunk->emit(OpCode::Jump, 0, node.line, node.column);
            loop.break_jumps.push_back(jump);
            break;
        }
        case NodeType::CONTINUE_STMT: {
            if (loops.empty()) {
                throw std::runtime_error("continue outside loop");
            }
            auto& loop = currentLoop();
            if (loop.has_continue_target) {
                chunk->emit(OpCode::Jump, loop.continue_target, node.line, node.column);
            } else {
                auto jump = chunk->emit(OpCode::Jump, 0, node.line, node.column);
                loop.continue_jumps.push_back(jump);
            }
            break;
        }
        default:
            compileExpression(node);
            break;
    }
}

void BytecodeCompiler::compileExpression(const MantraNode& node) {
    switch (node.type) {
        case NodeType::NUMBER_LIT: {
            const auto& number = static_cast<const NumberLitNode&>(node);
            emitConstant(MantraValue::number(number.value), node.line, node.column);
            break;
        }
        case NodeType::STRING_LIT: {
            const auto& string = static_cast<const StringLitNode&>(node);
            emitConstant(MantraValue::string(string.value), node.line, node.column);
            break;
        }
        case NodeType::BOOL_LIT: {
            const auto& boolean = static_cast<const BoolLitNode&>(node);
            emitConstant(MantraValue::boolean(boolean.value), node.line, node.column);
            break;
        }
        case NodeType::NULL_LIT:
            chunk->emit(OpCode::Null, 0, node.line, node.column);
            break;
        case NodeType::IDENTIFIER: {
            const auto& ident = static_cast<const IdentifierNode&>(node);
            emitName(OpCode::GetName, ident.name, node.line, node.column);
            break;
        }
        case NodeType::ARRAY_LIT: {
            const auto& array = static_cast<const ArrayLitNode&>(node);
            for (const auto& element : array.elements) {
                if (element) {
                    compileExpression(*element);
                } else {
                    chunk->emit(OpCode::Null, 0, node.line, node.column);
                }
            }
            chunk->emit(OpCode::BuildArray, array.elements.size(), node.line, node.column);
            break;
        }
        case NodeType::INDEX_EXPR: {
            const auto& index = static_cast<const IndexExprNode&>(node);
            if (index.collection) {
                compileExpression(*index.collection);
            }
            if (index.index) {
                compileExpression(*index.index);
            }
            chunk->emit(OpCode::GetIndex, 0, node.line, node.column);
            break;
        }
        case NodeType::UNARY_EXPR: {
            const auto& unary = static_cast<const UnaryExprNode&>(node);
            if (unary.operand) {
                compileExpression(*unary.operand);
            } else {
                chunk->emit(OpCode::Null, 0, node.line, node.column);
            }
            if (unary.op == TokenType::OP_MINUS) {
                chunk->emit(OpCode::Negate, 0, node.line, node.column);
            } else {
                chunk->emit(OpCode::Not, 0, node.line, node.column);
            }
            break;
        }
        case NodeType::BINARY_EXPR: {
            const auto& binary = static_cast<const BinaryExprNode&>(node);
            if (binary.op == TokenType::OP_AND) {
                compileExpression(*binary.left);
                auto jump_false = chunk->emit(OpCode::JumpIfFalse, 0, node.line, node.column);
                chunk->emit(OpCode::Pop, 0, node.line, node.column);
                compileExpression(*binary.right);
                emitBooleanize(node.line, node.column);
                auto jump_end = chunk->emit(OpCode::Jump, 0, node.line, node.column);
                patchJump(jump_false);
                chunk->emit(OpCode::Pop, 0, node.line, node.column);
                chunk->emit(OpCode::FalseValue, 0, node.line, node.column);
                patchJump(jump_end);
                break;
            }
            if (binary.op == TokenType::OP_OR) {
                compileExpression(*binary.left);
                auto jump_false = chunk->emit(OpCode::JumpIfFalse, 0, node.line, node.column);
                chunk->emit(OpCode::Pop, 0, node.line, node.column);
                chunk->emit(OpCode::TrueValue, 0, node.line, node.column);
                auto jump_end = chunk->emit(OpCode::Jump, 0, node.line, node.column);
                patchJump(jump_false);
                chunk->emit(OpCode::Pop, 0, node.line, node.column);
                compileExpression(*binary.right);
                emitBooleanize(node.line, node.column);
                patchJump(jump_end);
                break;
            }
            if (binary.left) {
                compileExpression(*binary.left);
            }
            if (binary.right) {
                compileExpression(*binary.right);
            }
            switch (binary.op) {
                case TokenType::OP_PLUS: chunk->emit(OpCode::Add, 0, node.line, node.column); break;
                case TokenType::OP_MINUS: chunk->emit(OpCode::Subtract, 0, node.line, node.column); break;
                case TokenType::OP_MULTIPLY: chunk->emit(OpCode::Multiply, 0, node.line, node.column); break;
                case TokenType::OP_DIVIDE: chunk->emit(OpCode::Divide, 0, node.line, node.column); break;
                case TokenType::OP_MODULO: chunk->emit(OpCode::Modulo, 0, node.line, node.column); break;
                case TokenType::OP_EQUAL: chunk->emit(OpCode::Equal, 0, node.line, node.column); break;
                case TokenType::OP_NOT_EQUAL: chunk->emit(OpCode::NotEqual, 0, node.line, node.column); break;
                case TokenType::OP_LESS: chunk->emit(OpCode::Less, 0, node.line, node.column); break;
                case TokenType::OP_LESS_EQUAL: chunk->emit(OpCode::LessEqual, 0, node.line, node.column); break;
                case TokenType::OP_GREATER: chunk->emit(OpCode::Greater, 0, node.line, node.column); break;
                case TokenType::OP_GREATER_EQUAL: chunk->emit(OpCode::GreaterEqual, 0, node.line, node.column); break;
                default:
                    throw std::runtime_error("Unsupported binary operator");
            }
            break;
        }
        case NodeType::CALL_EXPR: {
            const auto& call = static_cast<const CallExprNode&>(node);
            if (call.callee) {
                compileExpression(*call.callee);
            } else {
                chunk->emit(OpCode::Null, 0, node.line, node.column);
            }
            for (const auto& arg : call.arguments) {
                if (arg) {
                    compileExpression(*arg);
                } else {
                    chunk->emit(OpCode::Null, 0, node.line, node.column);
                }
            }
            chunk->emit(OpCode::Call, call.arguments.size(), node.line, node.column);
            break;
        }
        default:
            compileStatement(node);
            break;
    }
}

void BytecodeCompiler::compileIf(const IfStmtNode& node) {
    if (node.condition) {
        compileExpression(*node.condition);
    } else {
        chunk->emit(OpCode::FalseValue, 0, node.line, node.column);
    }
    auto jump_to_else = chunk->emit(OpCode::JumpIfFalse, 0, node.line, node.column);
    chunk->emit(OpCode::Pop, 0, node.line, node.column);
    if (node.then_branch) {
        compileStatement(*node.then_branch);
    }
    auto jump_to_end = chunk->emit(OpCode::Jump, 0, node.line, node.column);
    patchJump(jump_to_else);
    chunk->emit(OpCode::Pop, 0, node.line, node.column);
    if (node.else_branch) {
        compileStatement(*node.else_branch);
    }
    patchJump(jump_to_end);
}

void BytecodeCompiler::compileWhile(const WhileStmtNode& node) {
    const std::size_t loop_start = chunk->size();
    if (node.condition) {
        compileExpression(*node.condition);
    } else {
        chunk->emit(OpCode::FalseValue, 0, node.line, node.column);
    }
    auto exit_jump = chunk->emit(OpCode::JumpIfFalse, 0, node.line, node.column);
    chunk->emit(OpCode::Pop, 0, node.line, node.column);
    pushLoop(loop_start, true, loop_start);
    if (node.body) {
        compileStatement(*node.body);
    }
    chunk->emit(OpCode::Loop, loop_start, node.line, node.column);
    patchJump(exit_jump);
    chunk->emit(OpCode::Pop, 0, node.line, node.column);
    for (auto jump : currentLoop().break_jumps) {
        patchJump(jump);
    }
    popLoop();
}

void BytecodeCompiler::compileFor(const ForStmtNode& node) {
    chunk->emit(OpCode::PushScope, 0, node.line, node.column);

    if (node.start) {
        compileExpression(*node.start);
    } else {
        chunk->emit(OpCode::Null, 0, node.line, node.column);
    }
    emitName(OpCode::DefineName, node.variable, node.line, node.column);

    std::string end_name = tempName("__for_end");
    std::string step_name = tempName("__for_step");

    if (node.end) {
        compileExpression(*node.end);
    } else {
        chunk->emit(OpCode::Null, 0, node.line, node.column);
    }
    emitName(OpCode::DefineName, end_name, node.line, node.column);

    if (node.step) {
        compileExpression(*node.step);
    } else {
        emitName(OpCode::GetName, node.variable, node.line, node.column);
        emitName(OpCode::GetName, end_name, node.line, node.column);
        chunk->emit(OpCode::LessEqual, 0, node.line, node.column);
        auto choose_default_step = chunk->emit(OpCode::JumpIfFalse, 0, node.line, node.column);
        chunk->emit(OpCode::Pop, 0, node.line, node.column);
        emitConstant(MantraValue::number(1.0), node.line, node.column);
        auto skip_negative = chunk->emit(OpCode::Jump, 0, node.line, node.column);
        patchJump(choose_default_step);
        chunk->emit(OpCode::Pop, 0, node.line, node.column);
        emitConstant(MantraValue::number(-1.0), node.line, node.column);
        patchJump(skip_negative);
    }
    emitName(OpCode::DefineName, step_name, node.line, node.column);

    const std::size_t loop_start = chunk->size();
    pushLoop(loop_start, false, 0);

    emitName(OpCode::GetName, step_name, node.line, node.column);
    emitConstant(MantraValue::number(0.0), node.line, node.column);
    chunk->emit(OpCode::Greater, 0, node.line, node.column);
    auto step_positive_jump = chunk->emit(OpCode::JumpIfFalse, 0, node.line, node.column);
    chunk->emit(OpCode::Pop, 0, node.line, node.column);
    emitName(OpCode::GetName, node.variable, node.line, node.column);
    emitName(OpCode::GetName, end_name, node.line, node.column);
    chunk->emit(OpCode::LessEqual, 0, node.line, node.column);
    auto after_direction_jump = chunk->emit(OpCode::Jump, 0, node.line, node.column);
    patchJump(step_positive_jump);
    chunk->emit(OpCode::Pop, 0, node.line, node.column);
    emitName(OpCode::GetName, node.variable, node.line, node.column);
    emitName(OpCode::GetName, end_name, node.line, node.column);
    chunk->emit(OpCode::GreaterEqual, 0, node.line, node.column);
    patchJump(after_direction_jump);

    auto exit_jump = chunk->emit(OpCode::JumpIfFalse, 0, node.line, node.column);
    chunk->emit(OpCode::Pop, 0, node.line, node.column);

    auto& loop = currentLoop();
    if (node.body) {
        compileStatement(*node.body);
    }

    const std::size_t continue_target = chunk->size();
    loop.has_continue_target = true;
    loop.continue_target = continue_target;
    for (auto jump : loop.continue_jumps) {
        patchJump(jump);
    }

    emitName(OpCode::GetName, node.variable, node.line, node.column);
    emitName(OpCode::GetName, step_name, node.line, node.column);
    chunk->emit(OpCode::Add, 0, node.line, node.column);
    emitName(OpCode::SetName, node.variable, node.line, node.column);
    chunk->emit(OpCode::Loop, loop_start, node.line, node.column);

    patchJump(exit_jump);
    chunk->emit(OpCode::Pop, 0, node.line, node.column);
    for (auto jump : loop.break_jumps) {
        patchJump(jump);
    }
    popLoop();
    chunk->emit(OpCode::PopScope, 0, node.line, node.column);
}

void BytecodeCompiler::compileFunction(const FuncDefNode& node) {
    auto function_chunk = std::make_shared<BytecodeChunk>(node.name);
    auto previous_chunk = chunk;
    auto previous_loops = loops;
    auto previous_temp = temp_counter;
    chunk = function_chunk;
    if (node.body) {
        compileStatement(*node.body);
    }
    chunk->emit(OpCode::Null, 0, node.line, node.column);
    chunk->emit(OpCode::Return, 0, node.line, node.column);

    auto func_value = std::make_shared<FunctionValue>();
    func_value->is_native = false;
    func_value->name = node.name;
    func_value->params = node.parameters;
    func_value->body = nullptr;
    func_value->closure.reset();
    func_value->bytecode = function_chunk;

    chunk = previous_chunk;
    loops = previous_loops;
    temp_counter = previous_temp;

    auto function_index = chunk->addConstant(MantraValue::functionValue(func_value));
    chunk->emit(OpCode::DefineFunction, function_index, node.line, node.column);
}

void BytecodeCompiler::emitConstant(const MantraValue& value, int line, int column) {
    auto index = chunk->addConstant(value);
    chunk->emit(OpCode::Constant, index, line, column);
}

void BytecodeCompiler::emitName(OpCode op, const std::string& name, int line, int column) {
    auto index = chunk->addConstant(MantraValue::string(name));
    chunk->emit(op, index, line, column);
}

void BytecodeCompiler::emitJumpPlaceholder(OpCode op, int line, int column) {
    chunk->emit(op, 0, line, column);
}

void BytecodeCompiler::patchJump(std::size_t instruction_index) {
    chunk->patchOperand(instruction_index, chunk->size());
}

std::size_t BytecodeCompiler::makeTempName() {
    return ++temp_counter;
}

std::string BytecodeCompiler::tempName(const std::string& prefix) {
    return prefix + "_" + std::to_string(makeTempName());
}

void BytecodeCompiler::pushLoop(std::size_t loop_start, bool has_continue_target, std::size_t continue_target) {
    loops.push_back(LoopContext{loop_start, continue_target, has_continue_target, {}, {}});
}

BytecodeCompiler::LoopContext& BytecodeCompiler::currentLoop() {
    if (loops.empty()) {
        throw std::runtime_error("No active loop");
    }
    return loops.back();
}

void BytecodeCompiler::popLoop() {
    if (!loops.empty()) {
        loops.pop_back();
    }
}

void BytecodeCompiler::emitBooleanize(int line, int column) {
    chunk->emit(OpCode::Not, 0, line, column);
    chunk->emit(OpCode::Not, 0, line, column);
}

} // namespace mantra
