#include "mantra/vm/bytecode.h"

#include <utility>

namespace mantra {

BytecodeProgram::BytecodeProgram() = default;

std::size_t BytecodeProgram::addConstant(Value value) {
    constants_.push_back(std::move(value));
    return constants_.size() - 1;
}

void BytecodeProgram::emit(OpCode opcode, std::size_t operand) {
    instructions_.push_back(Instruction{opcode, operand});
}

std::size_t BytecodeProgram::emitJump(OpCode opcode) {
    emit(opcode, 0);
    return instructions_.size() - 1;
}

void BytecodeProgram::patchJump(std::size_t offset) {
    if (offset < instructions_.size()) {
        instructions_[offset].operand = instructions_.size();
    }
}

std::size_t BytecodeProgram::currentOffset() const {
    return instructions_.size();
}

const std::vector<Value>& BytecodeProgram::constants() const {
    return constants_;
}

const std::vector<Instruction>& BytecodeProgram::instructions() const {
    return instructions_;
}

int BytecodeProgram::addLocal(const std::string& name) {
    local_names_.push_back(name);
    local_offsets_.push_back(scope_depth_);
    return local_names_.size() - 1;
}

int BytecodeProgram::resolveLocal(const std::string& name) const {
    for (int i = static_cast<int>(local_names_.size()) - 1; i >= 0; --i) {
        if (local_names_[i] == name) {
            return i;
        }
    }
    return -1;
}

int BytecodeProgram::addGlobal(const std::string& name) {
    if (global_names_.find(name) == global_names_.end()) {
        global_names_[name] = global_names_.size();
    }
    return global_names_[name];
}

int BytecodeProgram::resolveGlobal(const std::string& name) const {
    auto it = global_names_.find(name);
    if (it != global_names_.end()) {
        return it->second;
    }
    return -1;
}

void BytecodeProgram::beginScope() {
    ++scope_depth_;
}

void BytecodeProgram::endScope() {
    // Remove locals from this scope
    while (!local_names_.empty() && local_offsets_.back() == scope_depth_) {
        local_names_.pop_back();
        local_offsets_.pop_back();
    }
    --scope_depth_;
}

int BytecodeProgram::scopeDepth() const {
    return scope_depth_;
}

} // namespace mantra
