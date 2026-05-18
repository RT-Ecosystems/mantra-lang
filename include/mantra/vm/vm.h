#pragma once

#include "bytecode.h"
#include "stack.h"

#include <ostream>
#include <string>

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
    std::string last_error_;

    StepResult executeInstruction(const BytecodeProgram& program,
                                  const Instruction& instruction,
                                  std::ostream& output);
    void fail(const std::string& message);
};

} // namespace mantra
