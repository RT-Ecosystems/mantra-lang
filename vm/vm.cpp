
#include "vm.h"
#include <iostream>
#include <vector>
#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <cmath>
#include <cstring>

namespace mantra {
namespace vm {

enum class OpCode : uint8_t {
    OP_CONSTANT = 0,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MODULO,
    OP_POWER,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,
    OP_RETURN,
    OP_CONCAT,
};

enum class ValueType { NIL, BOOLEAN, NUMBER, STRING };

struct Value {
    ValueType type;
    union { bool boolean; double number; } as;
    std::string string_value;

    Value() : type(ValueType::NIL) {}
    static Value nil() { return Value(); }
    static Value boolean(bool b) { Value v; v.type = ValueType::BOOLEAN; v.as.boolean = b; return v; }
    static Value number(double n) { Value v; v.type = ValueType::NUMBER; v.as.number = n; return v; }
    static Value string(const std::string& s) { Value v; v.type = ValueType::STRING; v.string_value = s; return v; }

    bool isNil() const { return type == ValueType::NIL; }
    bool isBoolean() const { return type == ValueType::BOOLEAN; }
    bool isNumber() const { return type == ValueType::NUMBER; }
    bool isString() const { return type == ValueType::STRING; }

    std::string toString() const {
        switch (type) {
            case ValueType::NIL: return "null";
            case ValueType::BOOLEAN: return as.boolean ? "true" : "false";
            case ValueType::NUMBER: { char buf[64]; snprintf(buf, sizeof(buf), "%g", as.number); return std::string(buf); }
            case ValueType::STRING: return string_value;
            default: return "<object>";
        }
    }

    bool isTruthy() const {
        if (type == ValueType::NIL) return false;
        if (type == ValueType::BOOLEAN) return as.boolean;
        return true;
    }
};

struct Chunk {
    std::vector<uint8_t> code;
    std::vector<Value> constants;
    std::vector<int> lines;

    void write(uint8_t byte, int line) {
        code.push_back(byte);
        lines.push_back(line);
    }
    size_t addConstant(const Value& value) {
        constants.push_back(value);
        return constants.size() - 1;
    }
};

struct CallFrame {
    size_t function_idx;
    size_t ip;
    size_t stack_base;
};

class VM {
public:
    std::vector<Value> stack;
    std::unordered_map<std::string, Value> globals;
    std::vector<CallFrame> frames;
    std::vector<Chunk> chunks;
    std::string last_error_;

    std::string lastError() const { return last_error_; }

    void push(const Value& value) { stack.push_back(value); }

    Value pop() {
        if (stack.empty()) throw std::runtime_error("Stack underflow");
        Value v = stack.back(); stack.pop_back(); return v;
    }

    Value& peek(size_t distance = 0) {
        if (stack.size() <= distance) throw std::runtime_error("Stack access out of bounds");
        return stack[stack.size() - 1 - distance];
    }

    bool execute(const Chunk& chunk, std::ostream& out) {
        try {
            chunks.push_back(chunk);
            size_t chunk_idx = chunks.size() - 1;
            CallFrame frame; frame.function_idx = chunk_idx; frame.ip = 0; frame.stack_base = 0;
            frames.push_back(frame);
            return run(out);
        } catch (const std::exception& e) {
            last_error_ = e.what(); return false;
        }
    }

    bool executeBytecode(const std::vector<uint8_t>& bytecode, std::ostream& out) {
        Chunk chunk; chunk.code = bytecode; return execute(chunk, out);
    }

private:
    bool run(std::ostream& out) {
        CallFrame& frame = frames.back();
        Chunk& chunk = chunks[frame.function_idx];

        #define READ_BYTE() (chunk.code[frame.ip++])
        #define READ_CONSTANT() (chunk.constants[READ_BYTE()])
        #define READ_SHORT() (frame.ip += 2, (uint16_t)((chunk.code[frame.ip - 2] << 8) | chunk.code[frame.ip - 1]))

        while (frame.ip < chunk.code.size()) {
            uint8_t instruction = READ_BYTE();

            switch (static_cast<OpCode>(instruction)) {
                case OpCode::OP_CONSTANT: push(READ_CONSTANT()); break;
                case OpCode::OP_NIL: push(Value::nil()); break;
                case OpCode::OP_TRUE: push(Value::boolean(true)); break;
                case OpCode::OP_FALSE: push(Value::boolean(false)); break;
                case OpCode::OP_POP: pop(); break;
                case OpCode::OP_GET_LOCAL: { uint8_t slot = READ_BYTE(); push(stack[frame.stack_base + slot]); break; }
                case OpCode::OP_SET_LOCAL: { uint8_t slot = READ_BYTE(); stack[frame.stack_base + slot] = peek(); break; }
                case OpCode::OP_DEFINE_GLOBAL: { Value name = READ_CONSTANT(); globals[name.string_value] = peek(); pop(); break; }
                case OpCode::OP_GET_GLOBAL: { Value name = READ_CONSTANT(); auto it = globals.find(name.string_value); if (it == globals.end()) throw std::runtime_error("Undefined variable: " + name.string_value); push(it->second); break; }
                case OpCode::OP_SET_GLOBAL: { Value name = READ_CONSTANT(); globals[name.string_value] = peek(); break; }
                case OpCode::OP_EQUAL: { Value b = pop(); Value a = pop(); push(Value::boolean(valuesEqual(a, b))); break; }
                case OpCode::OP_GREATER: { Value b = pop(); Value a = pop(); if (!a.isNumber() || !b.isNumber()) throw std::runtime_error("Operands must be numbers"); push(Value::boolean(a.as.number > b.as.number)); break; }
                case OpCode::OP_LESS: { Value b = pop(); Value a = pop(); if (!a.isNumber() || !b.isNumber()) throw std::runtime_error("Operands must be numbers"); push(Value::boolean(a.as.number < b.as.number)); break; }
                case OpCode::OP_ADD: { Value b = pop(); Value a = pop(); if (a.isNumber() && b.isNumber()) push(Value::number(a.as.number + b.as.number)); else if (a.isString() && b.isString()) push(Value::string(a.string_value + b.string_value)); else throw std::runtime_error("Operands must be two numbers or two strings"); break; }
                case OpCode::OP_SUBTRACT: { Value b = pop(); Value a = pop(); if (!a.isNumber() || !b.isNumber()) throw std::runtime_error("Operands must be numbers"); push(Value::number(a.as.number - b.as.number)); break; }
                case OpCode::OP_MULTIPLY: { Value b = pop(); Value a = pop(); if (!a.isNumber() || !b.isNumber()) throw std::runtime_error("Operands must be numbers"); push(Value::number(a.as.number * b.as.number)); break; }
                case OpCode::OP_DIVIDE: { Value b = pop(); Value a = pop(); if (!a.isNumber() || !b.isNumber()) throw std::runtime_error("Operands must be numbers"); if (b.as.number == 0) throw std::runtime_error("Division by zero"); push(Value::number(a.as.number / b.as.number)); break; }
                case OpCode::OP_MODULO: { Value b = pop(); Value a = pop(); if (!a.isNumber() || !b.isNumber()) throw std::runtime_error("Operands must be numbers"); push(Value::number(fmod(a.as.number, b.as.number))); break; }
                case OpCode::OP_POWER: { Value b = pop(); Value a = pop(); if (!a.isNumber() || !b.isNumber()) throw std::runtime_error("Operands must be numbers"); push(Value::number(pow(a.as.number, b.as.number))); break; }
                case OpCode::OP_NOT: push(Value::boolean(!pop().isTruthy())); break;
                case OpCode::OP_NEGATE: { Value v = pop(); if (!v.isNumber()) throw std::runtime_error("Operand must be a number"); push(Value::number(-v.as.number)); break; }
                case OpCode::OP_PRINT: out << pop().toString() << std::endl; break;
                case OpCode::OP_JUMP: { uint16_t offset = READ_SHORT(); frame.ip += offset; break; }
                case OpCode::OP_JUMP_IF_FALSE: { uint16_t offset = READ_SHORT(); if (!peek().isTruthy()) frame.ip += offset; break; }
                case OpCode::OP_LOOP: { uint16_t offset = READ_SHORT(); frame.ip -= offset; break; }
                case OpCode::OP_RETURN: return true;
                case OpCode::OP_CONCAT: { Value b = pop(); Value a = pop(); push(Value::string(a.toString() + b.toString())); break; }
                default: throw std::runtime_error("Unknown opcode: " + std::to_string(instruction));
            }
        }

        #undef READ_BYTE
        #undef READ_CONSTANT
        #undef READ_SHORT

        return true;
    }

    bool valuesEqual(const Value& a, const Value& b) {
        if (a.type != b.type) return false;
        switch (a.type) {
            case ValueType::NIL: return true;
            case ValueType::BOOLEAN: return a.as.boolean == b.as.boolean;
            case ValueType::NUMBER: return a.as.number == b.as.number;
            case ValueType::STRING: return a.string_value == b.string_value;
            default: return false;
        }
    }
};

} // namespace vm
} // namespace mantra
