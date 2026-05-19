#include "compiler.h"

#include <stdexcept>

namespace mantra {

BytecodeCompiler::BytecodeCompiler() = default;

std::shared_ptr<BytecodeChunk> BytecodeCompiler::compile(const ProgramNode&) {
    throw std::logic_error("BytecodeCompiler is not wired into the current pipeline");
}

} // namespace mantra
