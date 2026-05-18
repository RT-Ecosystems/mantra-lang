#include "chunk.h"

namespace mantra {
namespace bytecode {

void Chunk::write(uint8_t byte, int line) {
    code.push_back(byte);
    lines.push_back(line);
}

int Chunk::addConstant(const ConstantValue& value) {
    constants.push_back(value);
    return static_cast<int>(constants.size() - 1);
}

const std::vector<uint8_t>& Chunk::getCode() const {
    return code;
}

const std::vector<int>& Chunk::getLines() const {
    return lines;
}

const std::vector<ConstantValue>& Chunk::getConstants() const {
    return constants;
}

void Chunk::clear() {
    code.clear();
    lines.clear();
    constants.clear();
}

} // namespace bytecode
} // namespace mantra
