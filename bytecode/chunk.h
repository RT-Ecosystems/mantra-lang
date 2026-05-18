#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include "opcode.h"

namespace mantra {
namespace bytecode {

using ConstantValue = std::variant<
    double,
    std::string,
    bool,
    std::nullptr_t
>;

class Chunk {
public:
    Chunk() = default;
    ~Chunk() = default;

    void write(uint8_t byte, int line);

    int addConstant(const ConstantValue& value);

    const std::vector<uint8_t>& getCode() const;
    const std::vector<int>& getLines() const;
    const std::vector<ConstantValue>& getConstants() const;

    void clear();

private:
    std::vector<uint8_t> code;
    std::vector<int> lines;
    std::vector<ConstantValue> constants;
};

} // namespace bytecode
} // namespace mantra
