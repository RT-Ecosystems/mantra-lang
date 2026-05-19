#include "vm.h"

#include <stdexcept>

namespace mantra::legacy_vm {

bool VM::execute(const std::shared_ptr<bytecode::Chunk>& chunk) {
    if (!chunk) {
        throw std::logic_error("legacy_vm::VM requires a bytecode chunk");
    }
    throw std::logic_error("legacy_vm::VM is not wired into the current pipeline");
}

} // namespace mantra::legacy_vm
