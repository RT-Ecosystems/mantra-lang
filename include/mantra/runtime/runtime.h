#pragma once

#include <string>
#include <memory>
#include <vector>
#include <ostream>
#include "types.h"

namespace mantra {

// Forward declaration
class ProgramNode;

namespace runtime {

// Unified runtime interface for executing MANTRA programs
class Runtime {
public:
    Runtime();
    ~Runtime();

    // Execute a parsed AST program
    bool executeProgram(const ProgramNode& program, std::ostream& output);

    // Execute bytecode (future enhancement)
    bool executeBytecode(const std::string& bytecode, std::ostream& output);

    // Get the last runtime error
    const std::string& lastError() const;

    // Access global environment
    std::shared_ptr<Environment> globals() const;

private:
    std::shared_ptr<Environment> globals_;
    std::string last_error_;
    
    void setError(const std::string& message);
};

} // namespace runtime
} // namespace mantra
