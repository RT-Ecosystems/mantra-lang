#pragma once

#include "value.h"

#include <cstddef>
#include <vector>

namespace mantra {

enum class OpCode {
    PUSH_CONST,
    ADD,
    SUB,
    PRINT,
    HALT
};

struct Instruction {
    OpCode opcode = OpCode::HALT;
    std::size_t operand = 0;
};

class BytecodeProgram {
public:
    std::size_t addConstant(Value value);
    void emit(OpCode opcode, std::size_t operand = 0);

    const std::vector<Value>& constants() const;
    const std::vector<Instruction>& instructions() const;

private:
    std::vector<Value> constants_;
    std::vector<Instruction> instructions_;
};

} // namespace mantra
