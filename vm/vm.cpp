#include "vm.h"

#include "../error/error.h"
#include "../stdlib/io.h"
#include "../stdlib/math.h"
#include "../stdlib/string.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <stdexcept>

namespace mantra {

VM::VM()
    : globals(std::make_shared<Environment>()),
      max_stack_depth(4096),
      max_call_depth(1024) {
    registerStdlib();
}

bool VM::execute(const std::shared_ptr<BytecodeChunk>& chunk) {
    if (!chunk) {
        return false;
    }
    reset();
    frames.push_back(Frame{chunk, 0, globals, {}});
    return run();
}

void VM::registerStdlib() {
    auto addNative = [&](const std::string& name,
                         const std::function<MantraValue(const std::vector<MantraValue>&)>& fn) {
        auto func_value = std::make_shared<FunctionValue>();
        func_value->is_native = true;
        func_value->name = name;
        func_value->native = fn;
        globals->define(name, MantraValue::functionValue(func_value));
    };

    const std::vector<std::string> print_aliases = {
        "print", "println", "dikhao", "kaado", "dakho", "dekhao", "batavo", "dakhav",
        "chupinchu", "toro", "kaaniku", "jadi", "nohole", "dekhaow", "dakhoi", "dekhau",
        "darshaya", "waatav", "dikhay", "wekho", "nungsi", "nangi", "dado"
    };
    for (const auto& name : print_aliases) {
        addNative(name, stdlib::builtinPrint);
    }

    addNative("input", stdlib::builtinInput);
    addNative("lo", stdlib::builtinInput);
    addNative("loo", stdlib::builtinInput);
    addNative("length", stdlib::builtinLength);
    addNative("lambai", stdlib::builtinLength);
    addNative("concat", stdlib::builtinConcat);
    addNative("jodo_shabd", stdlib::builtinConcat);
    addNative("toNumber", stdlib::builtinToNumber);
    addNative("sankhya", stdlib::builtinToNumber);
    addNative("toString", stdlib::builtinToString);
    addNative("shabd", stdlib::builtinToString);
    addNative("type", stdlib::builtinType);
    addNative("exit", stdlib::builtinExit);
    addNative("roke", stdlib::builtinExit);
    addNative("clear", stdlib::builtinClear);
    addNative("saaf", stdlib::builtinClear);
    addNative("upper", stdlib::builtinUpper);
    addNative("lower", stdlib::builtinLower);
    addNative("trim", stdlib::builtinTrim);
    addNative("split", stdlib::builtinSplit);
    addNative("replace", stdlib::builtinReplace);
    addNative("slice", stdlib::builtinSlice);
    addNative("contains", stdlib::builtinContains);
    addNative("startsWith", stdlib::builtinStartsWith);
    addNative("endsWith", stdlib::builtinEndsWith);
    addNative("repeat", stdlib::builtinRepeat);

    addNative("sin", stdlib::builtinSin);
    addNative("cos", stdlib::builtinCos);
    addNative("tan", stdlib::builtinTan);
    addNative("asin", stdlib::builtinAsin);
    addNative("acos", stdlib::builtinAcos);
    addNative("atan", stdlib::builtinAtan);
    addNative("log", stdlib::builtinLog);
    addNative("log10", stdlib::builtinLog10);
    addNative("exp", stdlib::builtinExp);
    addNative("pow", stdlib::builtinPow);
    addNative("abs", stdlib::builtinAbs);
    addNative("sqrt", stdlib::builtinSqrt);
    addNative("round", stdlib::builtinRound);
    addNative("ceil", stdlib::builtinCeil);
    addNative("floor", stdlib::builtinFloor);
    addNative("random", stdlib::builtinRandom);
    addNative("pi", stdlib::builtinPi);
    addNative("e", stdlib::builtinE);
    addNative("infinity", stdlib::builtinInfinity);
    addNative("nan", stdlib::builtinNaN);
    addNative("is_infinity", stdlib::builtinIsInfinity);
    addNative("is_nan", stdlib::builtinIsNaN);
}

void VM::reset() {
    stack.clear();
    frames.clear();
}

bool VM::run() {
    while (!frames.empty()) {
        Frame& frame = frames.back();
        if (frame.ip >= frame.chunk->size()) {
            MantraValue result = MantraValue::nullValue();
            frames.pop_back();
            if (!finishCall(result)) {
                return false;
            }
            continue;
        }

        const Instruction& instruction = frame.chunk->at(frame.ip++);
        switch (instruction.op) {
            case OpCode::Constant: {
                const auto& constants = frame.chunk->constants();
                if (instruction.operand >= constants.size()) {
                    runtimeError("Constant index out of bounds", instruction);
                    return false;
                }
                push(constants[instruction.operand]);
                break;
            }
            case OpCode::Null:
                push(MantraValue::nullValue());
                break;
            case OpCode::TrueValue:
                push(MantraValue::boolean(true));
                break;
            case OpCode::FalseValue:
                push(MantraValue::boolean(false));
                break;
            case OpCode::Pop:
                if (stack.empty()) {
                    runtimeError("Stack underflow", instruction);
                    return false;
                }
                stack.pop_back();
                break;
            case OpCode::PushScope:
                frame.scopes.push_back(frame.environment);
                frame.environment = std::make_shared<Environment>(frame.environment);
                break;
            case OpCode::PopScope:
                if (frame.scopes.empty()) {
                    runtimeError("Scope underflow", instruction);
                    return false;
                }
                frame.environment = frame.scopes.back();
                frame.scopes.pop_back();
                break;
            case OpCode::GetName: {
                const auto& constants = frame.chunk->constants();
                if (instruction.operand >= constants.size() || constants[instruction.operand].type != ValueType::String) {
                    runtimeError("Invalid name constant", instruction);
                    return false;
                }
                const auto& name = constants[instruction.operand].string_value;
                try {
                    push(frame.environment->get(name));
                } catch (const std::exception& error) {
                    runtimeError(std::string("Variable lookup failed: ") + error.what(), instruction);
                    return false;
                }
                break;
            }
            case OpCode::DefineName: {
                const auto& constants = frame.chunk->constants();
                if (instruction.operand >= constants.size() || constants[instruction.operand].type != ValueType::String) {
                    runtimeError("Invalid name constant", instruction);
                    return false;
                }
                const auto& name = constants[instruction.operand].string_value;
                MantraValue value = pop();
                frame.environment->define(name, value);
                break;
            }
            case OpCode::SetName: {
                const auto& constants = frame.chunk->constants();
                if (instruction.operand >= constants.size() || constants[instruction.operand].type != ValueType::String) {
                    runtimeError("Invalid name constant", instruction);
                    return false;
                }
                const auto& name = constants[instruction.operand].string_value;
                MantraValue value = peek();
                if (!frame.environment->set(name, value)) {
                    frame.environment->define(name, value);
                }
                break;
            }
            case OpCode::DefineFunction: {
                const auto& constants = frame.chunk->constants();
                if (instruction.operand >= constants.size() || constants[instruction.operand].type != ValueType::Function) {
                    runtimeError("Invalid function constant", instruction);
                    return false;
                }
                auto function = constants[instruction.operand].function;
                if (!function) {
                    runtimeError("Missing function value", instruction);
                    return false;
                }
                auto bound = std::make_shared<FunctionValue>(*function);
                bound->closure = frame.environment;
                MantraValue value = MantraValue::functionValue(bound);
                frame.environment->define(bound->name, value);
                break;
            }
            case OpCode::Add:
            case OpCode::Subtract:
            case OpCode::Multiply:
            case OpCode::Divide:
            case OpCode::Modulo:
            case OpCode::Equal:
            case OpCode::NotEqual:
            case OpCode::Less:
            case OpCode::LessEqual:
            case OpCode::Greater:
            case OpCode::GreaterEqual: {
                MantraValue right = pop();
                MantraValue left = pop();
                MantraValue result = MantraValue::nullValue();
                switch (instruction.op) {
                    case OpCode::Add:
                        if (left.type == ValueType::Number && right.type == ValueType::Number) {
                            result = MantraValue::number(left.number_value + right.number_value);
                        } else {
                            result = MantraValue::string(left.toString() + right.toString());
                        }
                        break;
                    case OpCode::Subtract:
                        if (left.type == ValueType::Number && right.type == ValueType::Number) {
                            result = MantraValue::number(left.number_value - right.number_value);
                        } else {
                            runtimeError("'-' requires numbers", instruction);
                            return false;
                        }
                        break;
                    case OpCode::Multiply:
                        if (left.type == ValueType::Number && right.type == ValueType::Number) {
                            result = MantraValue::number(left.number_value * right.number_value);
                        } else {
                            runtimeError("'*' requires numbers", instruction);
                            return false;
                        }
                        break;
                    case OpCode::Divide:
                        if (left.type == ValueType::Number && right.type == ValueType::Number) {
                            if (std::abs(right.number_value) < 1e-12) {
                                runtimeError("Division by zero", instruction);
                                return false;
                            }
                            result = MantraValue::number(left.number_value / right.number_value);
                        } else {
                            runtimeError("'/' requires numbers", instruction);
                            return false;
                        }
                        break;
                    case OpCode::Modulo:
                        if (left.type == ValueType::Number && right.type == ValueType::Number) {
                            if (std::abs(right.number_value) < 1e-12) {
                                runtimeError("Division by zero", instruction);
                                return false;
                            }
                            result = MantraValue::number(std::fmod(left.number_value, right.number_value));
                        } else {
                            runtimeError("'%' requires numbers", instruction);
                            return false;
                        }
                        break;
                    case OpCode::Equal:
                        result = MantraValue::boolean(valuesEqual(left, right));
                        break;
                    case OpCode::NotEqual:
                        result = MantraValue::boolean(!valuesEqual(left, right));
                        break;
                    case OpCode::Less:
                        if (left.type == ValueType::Number && right.type == ValueType::Number) {
                            result = MantraValue::boolean(left.number_value < right.number_value);
                        } else {
                            runtimeError("'<' requires numbers", instruction);
                            return false;
                        }
                        break;
                    case OpCode::LessEqual:
                        if (left.type == ValueType::Number && right.type == ValueType::Number) {
                            result = MantraValue::boolean(left.number_value <= right.number_value);
                        } else {
                            runtimeError("'<=' requires numbers", instruction);
                            return false;
                        }
                        break;
                    case OpCode::Greater:
                        if (left.type == ValueType::Number && right.type == ValueType::Number) {
                            result = MantraValue::boolean(left.number_value > right.number_value);
                        } else {
                            runtimeError("'>' requires numbers", instruction);
                            return false;
                        }
                        break;
                    case OpCode::GreaterEqual:
                        if (left.type == ValueType::Number && right.type == ValueType::Number) {
                            result = MantraValue::boolean(left.number_value >= right.number_value);
                        } else {
                            runtimeError("'>=' requires numbers", instruction);
                            return false;
                        }
                        break;
                    default:
                        break;
                }
                push(result);
                break;
            }
            case OpCode::Negate: {
                MantraValue value = pop();
                if (value.type != ValueType::Number) {
                    runtimeError("Unary '-' requires a number", instruction);
                    return false;
                }
                push(MantraValue::number(-value.number_value));
                break;
            }
            case OpCode::Not: {
                MantraValue value = pop();
                push(MantraValue::boolean(!isTruthy(value)));
                break;
            }
            case OpCode::Jump:
                frame.ip = instruction.operand;
                break;
            case OpCode::JumpIfFalse: {
                if (!isTruthy(peek())) {
                    frame.ip = instruction.operand;
                }
                break;
            }
            case OpCode::Loop:
                frame.ip = instruction.operand;
                break;
            case OpCode::Print: {
                MantraValue value = pop();
                std::cout << value.toString() << std::endl;
                break;
            }
            case OpCode::BuildArray: {
                if (instruction.operand > stack.size()) {
                    runtimeError("Array build underflow", instruction);
                    return false;
                }
                std::vector<MantraValue> elements;
                elements.reserve(instruction.operand);
                for (std::size_t i = 0; i < instruction.operand; ++i) {
                    elements.push_back(pop());
                }
                std::reverse(elements.begin(), elements.end());
                push(MantraValue::array(std::move(elements)));
                break;
            }
            case OpCode::GetIndex: {
                MantraValue index_value = pop();
                MantraValue target = pop();
                if (index_value.type != ValueType::Number) {
                    runtimeError("Index must be numeric", instruction);
                    return false;
                }
                const int index = static_cast<int>(index_value.number_value);
                if (target.type == ValueType::Array) {
                    if (index < 0 || static_cast<std::size_t>(index) >= target.array_value.size()) {
                        runtimeError("Array index out of bounds", instruction);
                        return false;
                    }
                    push(target.array_value[static_cast<std::size_t>(index)]);
                } else if (target.type == ValueType::String) {
                    if (index < 0 || static_cast<std::size_t>(index) >= target.string_value.size()) {
                        runtimeError("String index out of bounds", instruction);
                        return false;
                    }
                    push(MantraValue::string(std::string(1, target.string_value[static_cast<std::size_t>(index)])));
                } else {
                    runtimeError("Indexing requires an array or string", instruction);
                    return false;
                }
                break;
            }
            case OpCode::Call: {
                if (instruction.operand > stack.size()) {
                    runtimeError("Call stack underflow", instruction);
                    return false;
                }
                std::vector<MantraValue> args;
                args.reserve(instruction.operand);
                for (std::size_t i = 0; i < instruction.operand; ++i) {
                    args.push_back(pop());
                }
                std::reverse(args.begin(), args.end());
                MantraValue callee = pop();
                if (!callValue(callee, args)) {
                    return false;
                }
                break;
            }
            case OpCode::Return: {
                MantraValue result = pop();
                frames.pop_back();
                if (!finishCall(result)) {
                    return false;
                }
                break;
            }
            default:
                runtimeError("Unsupported opcode", instruction);
                return false;
        }
    }
    return true;
}

void VM::push(const MantraValue& value) {
    if (stack.size() >= max_stack_depth) {
        throw std::runtime_error("Stack overflow");
    }
    stack.push_back(value);
}

MantraValue VM::pop() {
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow");
    }
    MantraValue value = stack.back();
    stack.pop_back();
    return value;
}

const MantraValue& VM::peek(std::size_t distance) const {
    if (distance >= stack.size()) {
        throw std::runtime_error("Stack underflow");
    }
    return stack[stack.size() - 1 - distance];
}

bool VM::isTruthy(const MantraValue& value) const {
    switch (value.type) {
        case ValueType::Boolean: return value.bool_value;
        case ValueType::Null: return false;
        case ValueType::Number: return std::abs(value.number_value) > 1e-12;
        case ValueType::String: return !value.string_value.empty();
        case ValueType::Function: return true;
        case ValueType::Array: return !value.array_value.empty();
        default: return false;
    }
}

bool VM::valuesEqual(const MantraValue& left, const MantraValue& right) const {
    if (left.type != right.type) {
        return false;
    }
    switch (left.type) {
        case ValueType::Number:
            return std::abs(left.number_value - right.number_value) < 1e-12;
        case ValueType::String:
            return left.string_value == right.string_value;
        case ValueType::Boolean:
            return left.bool_value == right.bool_value;
        case ValueType::Null:
            return true;
        case ValueType::Function:
            return left.function == right.function;
        case ValueType::Array:
            if (left.array_value.size() != right.array_value.size()) {
                return false;
            }
            for (std::size_t i = 0; i < left.array_value.size(); ++i) {
                if (!valuesEqual(left.array_value[i], right.array_value[i])) {
                    return false;
                }
            }
            return true;
        default:
            return false;
    }
}

bool VM::callValue(const MantraValue& callee, const std::vector<MantraValue>& args) {
    if (callee.type != ValueType::Function || !callee.function) {
        runtimeError("Value is not callable");
        return false;
    }

    auto function = callee.function;

    if (function->is_native) {
        push(function->native(args));
        return true;
    }

    if (!function->bytecode) {
        runtimeError("Function has no bytecode");
        return false;
    }

    if (args.size() != function->params.size()) {
        runtimeError("Function '" + function->name + "' expects " +
                     std::to_string(function->params.size()) + " arguments but got " +
                     std::to_string(args.size()));
        return false;
    }

    if (frames.size() >= max_call_depth) {
        runtimeError("Maximum call depth exceeded");
        return false;
    }

    auto env = std::make_shared<Environment>(function->closure);
    for (std::size_t i = 0; i < function->params.size(); ++i) {
        env->define(function->params[i], args[i]);
    }

    frames.push_back(Frame{function->bytecode, 0, env, {}});
    return true;
}

bool VM::finishCall(const MantraValue& result) {
    if (!frames.empty()) {
        push(result);
    }
    return true;
}

void VM::runtimeError(const std::string& message, const Instruction& instruction) const {
    ErrorHandler::printErrorWithContext(ErrorType::RUNTIME_ERROR,
                                        message,
                                        opCodeToString(instruction.op),
                                        instruction.line,
                                        instruction.column);
}

void VM::runtimeError(const std::string& message) const {
    ErrorHandler::printError(ErrorType::RUNTIME_ERROR, message, 0, 0);
}

} // namespace mantra
