#pragma once

#include "../bytecode/chunk.h"
#include "../core/ast.h"

#include <memory>

namespace mantra {

using BytecodeChunk = bytecode::Chunk;

class BytecodeCompiler {
public:
    BytecodeCompiler();

    std::shared_ptr<BytecodeChunk> compile(const ProgramNode& program);
};

} // namespace mantra
