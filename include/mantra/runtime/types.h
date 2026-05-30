#pragma once

#include <cstddef>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace mantra {
namespace runtime {

// Unified value type enumeration
enum class ValueType {
    Number,
    String,
    Boolean,
    Null,
    Function,
    Array,
    Object
};

// Forward declarations
class Value;
class Environment;

// Function value representation
struct Function {
    std::string name;
    std::vector<std::string> params;
    bool is_native = false;
    std::function<Value(const std::vector<Value>&)> native_fn;
    // For interpreter-based functions
    void* body_ptr = nullptr;  // Points to BlockStmtNode for interpreter functions
    std::shared_ptr<Environment> closure;
};

// Unified value representation
class Value {
public:
    // Default constructor
    Value();

    // Factory methods
    static Value number(double value);
    static Value string(std::string value);
    static Value boolean(bool value);
    static Value null();
    static Value function(const Function& fn);
    static Value array(const std::vector<Value>& elements);
    static Value object(const std::unordered_map<std::string, Value>& properties);

    // Type checking
    ValueType type() const;
    bool isNumber() const;
    bool isString() const;
    bool isBoolean() const;
    bool isNull() const;
    bool isFunction() const;
    bool isArray() const;
    bool isObject() const;

    // Value access
    double asNumber() const;
    const std::string& asString() const;
    bool asBoolean() const;
    const Function& asFunction() const;
    const std::vector<Value>& asArray() const;
    const std::unordered_map<std::string, Value>& asObject() const;

    // Conversion
    std::string toString() const;
    bool toBoolean() const;

    // Equality
    bool equals(const Value& other) const;
    bool operator==(const Value& other) const { return equals(other); }
    bool operator!=(const Value& other) const { return !equals(other); }

private:
    ValueType type_;
    double number_value_ = 0.0;
    std::string string_value_;
    bool bool_value_ = false;
    std::vector<Value> array_value_;
    std::shared_ptr<std::unordered_map<std::string, Value>> object_value_;
    std::shared_ptr<Function> function_value_;

    explicit Value(ValueType type);
};

// Environment for variable scoping
class Environment {
public:
    explicit Environment(std::shared_ptr<Environment> parent = nullptr);
    
    void define(const std::string& name, const Value& value);
    bool set(const std::string& name, const Value& value);
    Value get(const std::string& name) const;
    bool exists(const std::string& name) const;

private:
    std::unordered_map<std::string, Value> values_;
    std::shared_ptr<Environment> parent_;
};

// Execution result
enum class ExecutionStatus {
    Success,
    Error,
    Halt,
    Return,
    Break,
    Continue
};

// Runtime exception wrapper
class RuntimeException : public std::exception {
public:
    explicit RuntimeException(const std::string& msg) : message_(msg) {}
    const char* what() const noexcept override { return message_.c_str(); }

private:
    std::string message_;
};

} // namespace runtime
} // namespace mantra
