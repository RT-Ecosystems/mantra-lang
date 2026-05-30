#pragma once

#include "bytecode.h"
#include "stack.h"

#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace mantra {

class VM {
public:
    bool execute(const BytecodeProgram& program, std::ostream& output);
    const std::string& lastError() const;

private:
    enum class StepResult {
        Continue,
        Halt,
        Error
    };

    Stack stack_;
    std::vector<Value> globals_;
    std::string last_error_;
    std::size_t ip_ = 0;
    
    StepResult executeInstruction(const BytecodeProgram& program,
                                  const Instruction& instruction,
                                  std::ostream& output);
    void fail(const std::string& message);
    
    bool isTruthy(const Value& value) const;
};

} // namespace mantra
