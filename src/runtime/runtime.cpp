#include "mantra/runtime/runtime.h"
#include "core/interpreter.h"
#include <sstream>

namespace mantra {
namespace runtime {

Runtime::Runtime() : globals_(std::make_shared<Environment>()) {}

Runtime::~Runtime() = default;

bool Runtime::executeProgram(const ProgramNode& program, std::ostream& output) {
    try {
        // Use the existing interpreter for AST execution
        mantra::Interpreter interpreter;
        interpreter.interpret(program);
        return true;
    } catch (const std::exception& e) {
        setError(e.what());
        return false;
    }
}

bool Runtime::executeBytecode(const std::string& bytecode, std::ostream& output) {
    // TODO: Implement bytecode execution
    setError("Bytecode execution not yet implemented");
    return false;
}

const std::string& Runtime::lastError() const {
    return last_error_;
}

std::shared_ptr<Environment> Runtime::globals() const {
    return globals_;
}

void Runtime::setError(const std::string& message) {
    last_error_ = message;
}

} // namespace runtime
} // namespace mantra
