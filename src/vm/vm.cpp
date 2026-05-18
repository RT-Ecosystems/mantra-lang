#include "mantra/vm/vm.h"

#include <stdexcept>

namespace mantra {

bool VM::execute(const BytecodeProgram& program, std::ostream& output) {
    stack_.clear();
    last_error_.clear();

    const auto& instructions = program.instructions();
    for (std::size_t ip = 0; ip < instructions.size(); ++ip) {
        if (!executeInstruction(program, instructions[ip], ip, output)) {
            return last_error_.empty();
        }
    }

    return last_error_.empty();
}

const std::string& VM::lastError() const {
    return last_error_;
}

bool VM::executeInstruction(const BytecodeProgram& program,
                            const Instruction& instruction,
                            std::size_t,
                            std::ostream& output) {
    try {
        switch (instruction.opcode) {
            case OpCode::PUSH_CONST: {
                const auto& constants = program.constants();
                if (instruction.operand >= constants.size()) {
                    fail("Constant index out of range");
                    return false;
                }
                stack_.push(constants[instruction.operand]);
                return true;
            }
            case OpCode::ADD: {
                const Value right = stack_.pop();
                const Value left = stack_.pop();
                if (!left.isNumber() || !right.isNumber()) {
                    fail("ADD expects two numbers");
                    return false;
                }
                stack_.push(Value::number(left.asNumber() + right.asNumber()));
                return true;
            }
            case OpCode::SUB: {
                const Value right = stack_.pop();
                const Value left = stack_.pop();
                if (!left.isNumber() || !right.isNumber()) {
                    fail("SUB expects two numbers");
                    return false;
                }
                stack_.push(Value::number(left.asNumber() - right.asNumber()));
                return true;
            }
            case OpCode::PRINT: {
                const Value value = stack_.pop();
                output << value.toString() << '\n';
                return true;
            }
            case OpCode::HALT:
                return false;
        }
    } catch (const std::exception& error) {
        fail(error.what());
        return false;
    }

    fail("Unknown instruction");
    return false;
}

void VM::fail(const std::string& message) {
    last_error_ = message;
}

} // namespace mantra
