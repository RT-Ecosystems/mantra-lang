#include "chunk.h"

#include <stdexcept>
#include <utility>

namespace mantra {

BytecodeChunk::BytecodeChunk(std::string name)
    : chunk_name(std::move(name)) {}

std::size_t BytecodeChunk::addConstant(const MantraValue& value) {
    constant_pool.push_back(value);
    return constant_pool.size() - 1;
}

std::size_t BytecodeChunk::emit(OpCode op, std::size_t operand, int line, int column) {
    code.push_back(Instruction{op, operand, line, column});
    return code.size() - 1;
}

void BytecodeChunk::patchOperand(std::size_t instruction_index, std::size_t operand) {
    if (instruction_index >= code.size()) {
        throw std::out_of_range("Invalid bytecode patch index");
    }
    code[instruction_index].operand = operand;
}

std::size_t BytecodeChunk::size() const {
    return code.size();
}

Instruction& BytecodeChunk::at(std::size_t index) {
    return code.at(index);
}

const Instruction& BytecodeChunk::at(std::size_t index) const {
    return code.at(index);
}

const std::vector<Instruction>& BytecodeChunk::instructions() const {
    return code;
}

std::vector<Instruction>& BytecodeChunk::instructions() {
    return code;
}

const std::vector<MantraValue>& BytecodeChunk::constants() const {
    return constant_pool;
}

std::vector<MantraValue>& BytecodeChunk::constants() {
    return constant_pool;
}

const std::string& BytecodeChunk::name() const {
    return chunk_name;
}

} // namespace mantra
