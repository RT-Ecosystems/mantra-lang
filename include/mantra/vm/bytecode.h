#pragma once

#include "value.h"
#include "../core/types.h"

#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace mantra {

// OpCode enum is defined in core/types.h

struct Instruction {
    OpCode opcode = OpCode::HALT;
    std::size_t operand = 0;
};

class BytecodeProgram {
public:
    BytecodeProgram();
    
    std::size_t addConstant(Value value);
    void emit(OpCode opcode, std::size_t operand = 0);
    
    std::size_t emitJump(OpCode opcode);
    void patchJump(std::size_t offset);
    
    std::size_t currentOffset() const;
    
    const std::vector<Value>& constants() const;
    const std::vector<Instruction>& instructions() const;
    
    // Variable management
    int addLocal(const std::string& name);
    int resolveLocal(const std::string& name) const;
    int addGlobal(const std::string& name);
    int resolveGlobal(const std::string& name) const;
    
    void beginScope();
    void endScope();
    int scopeDepth() const;
    
private:
    std::vector<Value> constants_;
    std::vector<Instruction> instructions_;
    
    // Scoping
    int scope_depth_ = 0;
    std::vector<int> local_offsets_;
    std::vector<std::string> local_names_;
    std::map<std::string, int> global_names_;
};

} // namespace mantra
