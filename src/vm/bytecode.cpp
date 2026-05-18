#include "mantra/vm/bytecode.h"

#include <utility>

namespace mantra {

std::size_t BytecodeProgram::addConstant(Value value) {
    constants_.push_back(std::move(value));
    return constants_.size() - 1;
}

void BytecodeProgram::emit(OpCode opcode, std::size_t operand) {
    instructions_.push_back(Instruction{opcode, operand});
}

const std::vector<Value>& BytecodeProgram::constants() const {
    return constants_;
}

const std::vector<Instruction>& BytecodeProgram::instructions() const {
    return instructions_;
}

} // namespace mantra
