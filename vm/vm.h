#pragma once

#include "../bytecode/chunk.h"

#include <memory>
#include <string>
#include <vector>

namespace mantra {

class VM {
public:
    VM();

    bool execute(const std::shared_ptr<BytecodeChunk>& chunk);

private:
    struct Frame {
        std::shared_ptr<BytecodeChunk> chunk;
        std::size_t ip = 0;
        std::shared_ptr<Environment> environment;
        std::vector<std::shared_ptr<Environment>> scopes;
    };

    std::vector<MantraValue> stack;
    std::vector<Frame> frames;
    std::shared_ptr<Environment> globals;
    std::size_t max_stack_depth;
    std::size_t max_call_depth;

    void registerStdlib();
    void reset();

    bool run();
    void push(const MantraValue& value);
    MantraValue pop();
    const MantraValue& peek(std::size_t distance = 0) const;
    bool isTruthy(const MantraValue& value) const;
    bool valuesEqual(const MantraValue& left, const MantraValue& right) const;

    bool callValue(const MantraValue& callee, const std::vector<MantraValue>& args);
    bool finishCall(const MantraValue& result);
    void runtimeError(const std::string& message, const Instruction& instruction) const;
    void runtimeError(const std::string& message) const;
};

} // namespace mantra
