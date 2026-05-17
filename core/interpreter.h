#ifndef MANTRA_INTERPRETER_H
#define MANTRA_INTERPRETER_H

#include "ast.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace mantra {

enum class ValueType {
    Number,
    String,
    Boolean,
    Null,
    Function
};

struct FunctionValue {
    bool is_native = false;
    std::string name;
    std::vector<std::string> params;
    const BlockNode* body = nullptr;
    std::shared_ptr<class Environment> closure;
    std::function<class MantraValue(const std::vector<class MantraValue>&)> native;
};

struct MantraValue {
    ValueType type = ValueType::Null;
    double number_value = 0.0;
    std::string string_value;
    bool bool_value = false;
    std::shared_ptr<FunctionValue> function;

    static MantraValue number(double value);
    static MantraValue string(const std::string& value);
    static MantraValue boolean(bool value);
    static MantraValue nullValue();
    static MantraValue functionValue(std::shared_ptr<FunctionValue> func);

    std::string toString() const;
};

class Environment {
public:
    explicit Environment(std::shared_ptr<Environment> parent_env = nullptr);

    void define(const std::string& name, const MantraValue& value);
    bool assign(const std::string& name, const MantraValue& value);
    MantraValue get(const std::string& name) const;

private:
    std::unordered_map<std::string, MantraValue> values;
    std::shared_ptr<Environment> parent;
};

class ReturnException : public std::exception {
public:
    explicit ReturnException(MantraValue value);
    const MantraValue& value() const;

private:
    MantraValue return_value;
};

class Interpreter {
public:
    Interpreter();

    void interpret(const ProgramNode& program);
    MantraValue evaluate(const MantraNode& node);

private:
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Environment> environment;

    void execute(const MantraNode& node);
    void executeBlock(const BlockNode& block, std::shared_ptr<Environment> new_env);

    MantraValue evaluateBinary(const BinaryNode& node);
    MantraValue evaluateUnary(const UnaryNode& node);
    MantraValue evaluateCall(const CallNode& node);

    bool isTruthy(const MantraValue& value) const;
    bool valuesEqual(const MantraValue& left, const MantraValue& right) const;
    void registerStdlib();

    void runtimeError(const std::string& message, const MantraNode& node) const;
};

} // namespace mantra

#endif // MANTRA_INTERPRETER_H
