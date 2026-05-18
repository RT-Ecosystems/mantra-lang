#pragma once

#include "opcode.h"
#include "../core/interpreter.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace mantra {

struct Instruction {
    OpCode op = OpCode::Null;
    std::size_t operand = 0;
    int line = 0;
    int column = 0;
};

class BytecodeChunk {
public:
    explicit BytecodeChunk(std::string chunk_name = "main");

    std::size_t addConstant(const MantraValue& value);
    std::size_t emit(OpCode op, std::size_t operand = 0, int line = 0, int column = 0);
    void patchOperand(std::size_t instruction_index, std::size_t operand);

    std::size_t size() const;
    Instruction& at(std::size_t index);
    const Instruction& at(std::size_t index) const;

    const std::vector<Instruction>& instructions() const;
    std::vector<Instruction>& instructions();
    const std::vector<MantraValue>& constants() const;
    std::vector<MantraValue>& constants();
    const std::string& name() const;

private:
    std::string chunk_name;
    std::vector<Instruction> code;
    std::vector<MantraValue> constant_pool;
};

} // namespace mantra
