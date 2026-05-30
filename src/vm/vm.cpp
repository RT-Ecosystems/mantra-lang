#include "mantra/vm/vm.h"

#include <cmath>
#include <stdexcept>

namespace mantra {

bool VM::execute(const BytecodeProgram& program, std::ostream& output) {
    stack_.clear();
    globals_.clear();
    last_error_.clear();
    ip_ = 0;

    const auto& instructions = program.instructions();
    while (ip_ < instructions.size()) {
        const StepResult result = executeInstruction(program, instructions[ip_], output);
        if (result == StepResult::Error) {
            return false;
        }
        if (result == StepResult::Halt) {
            return true;
        }
        ++ip_;
    }

    return true;
}

const std::string& VM::lastError() const {
    return last_error_;
}

VM::StepResult VM::executeInstruction(const BytecodeProgram& program,
                                      const Instruction& instruction,
                                      std::ostream& output) {
    try {
        switch (instruction.opcode) {
            case OpCode::PUSH_CONST: {
                const auto& constants = program.constants();
                if (instruction.operand >= constants.size()) {
                    fail("Constant index out of range");
                    return StepResult::Error;
                }
                stack_.push(constants[instruction.operand]);
                return StepResult::Continue;
            }
            
            case OpCode::POP: {
                if (stack_.empty()) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                stack_.pop();
                return StepResult::Continue;
            }
            
            case OpCode::ADD: {
                if (stack_.size() < 2) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value right = stack_.pop();
                const Value left = stack_.pop();
                if (!left.isNumber() || !right.isNumber()) {
                    fail("ADD expects two numbers");
                    return StepResult::Error;
                }
                stack_.push(Value::number(left.asNumber() + right.asNumber()));
                return StepResult::Continue;
            }
            
            case OpCode::SUB: {
                if (stack_.size() < 2) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value right = stack_.pop();
                const Value left = stack_.pop();
                if (!left.isNumber() || !right.isNumber()) {
                    fail("SUB expects two numbers");
                    return StepResult::Error;
                }
                stack_.push(Value::number(left.asNumber() - right.asNumber()));
                return StepResult::Continue;
            }
            
            case OpCode::MUL: {
                if (stack_.size() < 2) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value right = stack_.pop();
                const Value left = stack_.pop();
                if (!left.isNumber() || !right.isNumber()) {
                    fail("MUL expects two numbers");
                    return StepResult::Error;
                }
                stack_.push(Value::number(left.asNumber() * right.asNumber()));
                return StepResult::Continue;
            }
            
            case OpCode::DIV: {
                if (stack_.size() < 2) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value right = stack_.pop();
                const Value left = stack_.pop();
                if (!left.isNumber() || !right.isNumber()) {
                    fail("DIV expects two numbers");
                    return StepResult::Error;
                }
                if (right.asNumber() == 0.0) {
                    fail("Division by zero");
                    return StepResult::Error;
                }
                stack_.push(Value::number(left.asNumber() / right.asNumber()));
                return StepResult::Continue;
            }
            
            case OpCode::NEGATE: {
                if (stack_.empty()) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value value = stack_.pop();
                if (!value.isNumber()) {
                    fail("NEGATE expects a number");
                    return StepResult::Error;
                }
                stack_.push(Value::number(-value.asNumber()));
                return StepResult::Continue;
            }
            
            case OpCode::TRUE: {
                stack_.push(Value::boolean(true));
                return StepResult::Continue;
            }
            
            case OpCode::FALSE: {
                stack_.push(Value::boolean(false));
                return StepResult::Continue;
            }
            
            case OpCode::NOT: {
                if (stack_.empty()) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value value = stack_.pop();
                stack_.push(Value::boolean(!isTruthy(value)));
                return StepResult::Continue;
            }
            
            case OpCode::EQUAL: {
                if (stack_.size() < 2) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value right = stack_.pop();
                const Value left = stack_.pop();
                stack_.push(Value::boolean(left == right));
                return StepResult::Continue;
            }
            
            case OpCode::NOT_EQUAL: {
                if (stack_.size() < 2) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value right = stack_.pop();
                const Value left = stack_.pop();
                stack_.push(Value::boolean(left != right));
                return StepResult::Continue;
            }
            
            case OpCode::GREATER: {
                if (stack_.size() < 2) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value right = stack_.pop();
                const Value left = stack_.pop();
                if (!left.isNumber() || !right.isNumber()) {
                    fail("GREATER expects two numbers");
                    return StepResult::Error;
                }
                stack_.push(Value::boolean(left.asNumber() > right.asNumber()));
                return StepResult::Continue;
            }
            
            case OpCode::GREATER_EQUAL: {
                if (stack_.size() < 2) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value right = stack_.pop();
                const Value left = stack_.pop();
                if (!left.isNumber() || !right.isNumber()) {
                    fail("GREATER_EQUAL expects two numbers");
                    return StepResult::Error;
                }
                stack_.push(Value::boolean(left.asNumber() >= right.asNumber()));
                return StepResult::Continue;
            }
            
            case OpCode::LESS: {
                if (stack_.size() < 2) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value right = stack_.pop();
                const Value left = stack_.pop();
                if (!left.isNumber() || !right.isNumber()) {
                    fail("LESS expects two numbers");
                    return StepResult::Error;
                }
                stack_.push(Value::boolean(left.asNumber() < right.asNumber()));
                return StepResult::Continue;
            }
            
            case OpCode::LESS_EQUAL: {
                if (stack_.size() < 2) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value right = stack_.pop();
                const Value left = stack_.pop();
                if (!left.isNumber() || !right.isNumber()) {
                    fail("LESS_EQUAL expects two numbers");
                    return StepResult::Error;
                }
                stack_.push(Value::boolean(left.asNumber() <= right.asNumber()));
                return StepResult::Continue;
            }
            
            case OpCode::DEFINE_GLOBAL: {
                if (stack_.empty()) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                globals_.push_back(stack_.pop());
                return StepResult::Continue;
            }
            
            case OpCode::GET_GLOBAL: {
                if (instruction.operand >= globals_.size()) {
                    fail("Global variable index out of range");
                    return StepResult::Error;
                }
                stack_.push(globals_[instruction.operand]);
                return StepResult::Continue;
            }
            
            case OpCode::SET_GLOBAL: {
                if (stack_.empty()) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                if (instruction.operand >= globals_.size()) {
                    fail("Global variable index out of range");
                    return StepResult::Error;
                }
                globals_[instruction.operand] = stack_.peek();
                return StepResult::Continue;
            }
            
            case OpCode::PRINT: {
                if (stack_.empty()) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value value = stack_.pop();
                output << value.toString() << '\n';
                return StepResult::Continue;
            }
            
            case OpCode::JUMP: {
                ip_ = instruction.operand - 1;
                return StepResult::Continue;
            }
            
            case OpCode::JUMP_IF_FALSE: {
                if (stack_.empty()) {
                    fail("Stack underflow");
                    return StepResult::Error;
                }
                const Value value = stack_.pop();
                if (!isTruthy(value)) {
                    ip_ = instruction.operand - 1;
                }
                return StepResult::Continue;
            }
            
            case OpCode::LOOP: {
                ip_ = instruction.operand - 1;
                return StepResult::Continue;
            }
            
            case OpCode::RETURN: {
                return StepResult::Halt;
            }
            
            case OpCode::HALT: {
                return StepResult::Halt;
            }
            
            default: {
                fail("Unknown instruction");
                return StepResult::Error;
            }
        }
    } catch (const std::exception& error) {
        fail(error.what());
        return StepResult::Error;
    }
}

void VM::fail(const std::string& message) {
    last_error_ = message;
}

bool VM::isTruthy(const Value& value) const {
    if (value.isNull()) {
        return false;
    }
    if (value.isBoolean()) {
        return value.asBoolean();
    }
    if (value.isNumber()) {
        return value.asNumber() != 0.0;
    }
    if (value.isString()) {
        return !value.asString().empty();
    }
    return true;
}

} // namespace mantra
