#pragma once

#include "../bytecode/chunk.h"

#include <memory>

namespace mantra::legacy_vm {

class VM {
public:
    VM() = default;
    bool execute(const std::shared_ptr<bytecode::Chunk>& chunk);
};

} // namespace mantra::legacy_vm
