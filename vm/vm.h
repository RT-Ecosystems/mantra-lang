
#pragma once

#include <vector>
#include <cstdint>
#include <iostream>
#include <string>

namespace mantra {
namespace vm {

struct Value;
struct Chunk;
class VM;

class VM {
public:
    VM();
    ~VM();
    bool execute(const Chunk& chunk, std::ostream& out);
    bool executeBytecode(const std::vector<uint8_t>& bytecode, std::ostream& out);
    std::string lastError() const;
private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace vm
} // namespace mantra
