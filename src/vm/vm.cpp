#include "mantra/vm/vm.h"

#include <stdexcept>

namespace mantra {

bool VM::execute(const BytecodeProgram& program, std::ostream& output) {
    stack_.clear();
    last_error_.clear();

    const auto& instructions = program.instructions();
    for (std::size_t ip = 0; ip < instructions.size(); ++ip) {
        const StepResult result = executeInstruction(program, instructions[ip], output);
        if (result == StepResult::Error) {
            return false;
        }
        if (result == StepResult::Halt) {
            return true;
        }
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
            case OpCode::ADD: {
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
                const Value right = stack_.pop();
                const Value left = stack_.pop();
                if (!left.isNumber() || !right.isNumber()) {
                    fail("SUB expects two numbers");
                    return StepResult::Error;
                }
                stack_.push(Value::number(left.asNumber() - right.asNumber()));
                return StepResult::Continue;
            }
            case OpCode::PRINT: {
                const Value value = stack_.pop();
                output << value.toString() << '\n';
                return StepResult::Continue;
            }
            case OpCode::HALT:
                return StepResult::Halt;
            default:
                fail("Unknown instruction");
                return StepResult::Error;
        }
    } catch (const std::exception& error) {
        fail(error.what());
        return StepResult::Error;
    }
}

void VM::fail(const std::string& message) {
    last_error_ = message;
}

} // namespace mantra
