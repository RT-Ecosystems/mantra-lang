#include "interpreter.h"
#include "error.h"
#include "io.h"
#include <cmath>
#include <iostream>
#include <sstream>

namespace mantra {

MantraValue MantraValue::number(double value) {
    MantraValue result;
    result.type = ValueType::Number;
    result.number_value = value;
    return result;
}

MantraValue MantraValue::string(const std::string& value) {
    MantraValue result;
    result.type = ValueType::String;
    result.string_value = value;
    return result;
}

MantraValue MantraValue::boolean(bool value) {
    MantraValue result;
    result.type = ValueType::Boolean;
    result.bool_value = value;
    return result;
}

MantraValue MantraValue::nullValue() {
    MantraValue result;
    result.type = ValueType::Null;
    return result;
}

MantraValue MantraValue::functionValue(std::shared_ptr<FunctionValue> func) {
    MantraValue result;
    result.type = ValueType::Function;
    result.function = std::move(func);
    return result;
}

std::string MantraValue::toString() const {
    std::ostringstream out;
    switch (type) {
        case ValueType::Number:
            out << number_value;
            return out.str();
        case ValueType::String:
            return string_value;
        case ValueType::Boolean:
            return bool_value ? "सच" : "झूठ";
        case ValueType::Null:
            return "null";
        case ValueType::Function:
            if (function) {
                return "<function " + function->name + ">";
            }
            return "<function>";
        default:
            return "<unknown>";
    }
}

Environment::Environment(std::shared_ptr<Environment> parent_env)
    : parent(std::move(parent_env)) {}

void Environment::define(const std::string& name, const MantraValue& value) {
    values[name] = value;
}

bool Environment::assign(const std::string& name, const MantraValue& value) {
    auto it = values.find(name);
    if (it != values.end()) {
        it->second = value;
        return true;
    }

    if (parent) {
        return parent->assign(name, value);
    }

    return false;
}

MantraValue Environment::get(const std::string& name) const {
    auto it = values.find(name);
    if (it != values.end()) {
        return it->second;
    }

    if (parent) {
        return parent->get(name);
    }

    ErrorHandler::throwError(ErrorType::UNKNOWN_IDENTIFIER,
                             "अपरिचित पहचानकर्ता: " + name,
                             0, 0, 1);
    return MantraValue::nullValue();
}

ReturnException::ReturnException(MantraValue value)
    : return_value(std::move(value)) {}

const MantraValue& ReturnException::value() const {
    return return_value;
}

Interpreter::Interpreter() {
    globals = std::make_shared<Environment>();
    environment = globals;
    registerStdlib();
}

void Interpreter::interpret(const ProgramNode& program) {
    for (const auto& stmt : program.statements) {
        execute(*stmt);
    }
}

MantraValue Interpreter::evaluate(const MantraNode& node) {
    switch (node.type) {
        case NodeType::NUMBER_LIT: {
            const auto& number_node = static_cast<const NumberLitNode&>(node);
            return MantraValue::number(number_node.value);
        }
        case NodeType::STRING_LIT: {
            const auto& string_node = static_cast<const StringLitNode&>(node);
            return MantraValue::string(string_node.value);
        }
        case NodeType::BOOL_LIT: {
            const auto& bool_node = static_cast<const BoolLitNode&>(node);
            return MantraValue::boolean(bool_node.value);
        }
        case NodeType::NULL_LIT:
            return MantraValue::nullValue();
        case NodeType::IDENTIFIER: {
            const auto& ident_node = static_cast<const IdentifierNode&>(node);
            return environment->get(ident_node.name);
        }
        case NodeType::BINARY_EXPR:
            return evaluateBinary(static_cast<const BinaryExprNode&>(node));
        case NodeType::UNARY_EXPR:
            return evaluateUnary(static_cast<const UnaryExprNode&>(node));
        case NodeType::CALL_EXPR:
            return evaluateCall(static_cast<const CallExprNode&>(node));
        case NodeType::ASSIGN_STMT: {
            const auto& assign_node = static_cast<const AssignStmtNode&>(node);
            MantraValue value = evaluate(*assign_node.value);
            if (!environment->assign(assign_node.name, value)) {
                environment->define(assign_node.name, value);
            }
            return value;
        }
        case NodeType::EXPR_STMT: {
            const auto& expr_node = static_cast<const ExprStmtNode&>(node);
            if (expr_node.expression) {
                return evaluate(*expr_node.expression);
            }
            return MantraValue::nullValue();
        }
        default:
            runtimeError("अमान्य अभिव्यक्ति", node);
            return MantraValue::nullValue();
    }
}

void Interpreter::execute(const MantraNode& node) {
    switch (node.type) {
        case NodeType::PROGRAM: {
            const auto& program = static_cast<const ProgramNode&>(node);
            interpret(program);
            break;
        }
        case NodeType::BLOCK_STMT: {
            const auto& block = static_cast<const BlockStmtNode&>(node);
            executeBlock(block, environment);
            break;
        }
        case NodeType::PRINT_STMT: {
            const auto& print_node = static_cast<const PrintStmtNode&>(node);
            MantraValue value = evaluate(*print_node.expression);
            std::cout << value.toString() << std::endl;
            break;
        }
        case NodeType::IF_STMT: {
            const auto& if_node = static_cast<const IfStmtNode&>(node);
            if (isTruthy(evaluate(*if_node.condition))) {
                executeBlock(*if_node.then_branch, environment);
            } else if (if_node.else_branch) {
                executeBlock(*if_node.else_branch, environment);
            }
            break;
        }
        case NodeType::WHILE_STMT: {
            const auto& while_node = static_cast<const WhileStmtNode&>(node);
            while (isTruthy(evaluate(*while_node.condition))) {
                executeBlock(*while_node.body, environment);
            }
            break;
        }
        case NodeType::FOR_STMT: {
            const auto& for_node = static_cast<const ForStmtNode&>(node);
            MantraValue start_val = evaluate(*for_node.start);
            MantraValue end_val = evaluate(*for_node.end);
            if (start_val.type != ValueType::Number || end_val.type != ValueType::Number) {
                runtimeError("for/बारबार में संख्या अपेक्षित है", node);
                return;
            }

            double start = start_val.number_value;
            double end = end_val.number_value;
            double step = (start <= end) ? 1.0 : -1.0;

            environment->define(for_node.variable, MantraValue::number(start));
            for (double i = start; (step > 0) ? i <= end : i >= end; i += step) {
                environment->assign(for_node.variable, MantraValue::number(i));
                executeBlock(*for_node.body, environment);
            }
            break;
        }
        case NodeType::FUNC_DEF: {
            const auto& func_node = static_cast<const FuncDefNode&>(node);
            auto func_value = std::make_shared<FunctionValue>();
            func_value->is_native = false;
            func_value->name = func_node.name;
            func_value->params = func_node.parameters;
            func_value->body = func_node.body.get();
            func_value->closure = environment;
            environment->define(func_node.name, MantraValue::functionValue(func_value));
            break;
        }
        case NodeType::RETURN_STMT: {
            const auto& return_node = static_cast<const ReturnStmtNode&>(node);
            MantraValue value = MantraValue::nullValue();
            if (return_node.value) {
                value = evaluate(*return_node.value);
            }
            throw ReturnException(value);
        }
        case NodeType::ASSIGN_STMT:
        case NodeType::BINARY_EXPR:
        case NodeType::UNARY_EXPR:
        case NodeType::CALL_EXPR:
        case NodeType::EXPR_STMT:
            evaluate(node);
            break;
        default:
            runtimeError("अमान्य स्टेटमेंट", node);
            break;
    }
}

void Interpreter::executeBlock(const BlockStmtNode& block, std::shared_ptr<Environment> new_env) {
    auto previous = environment;
    environment = std::move(new_env);
    try {
        for (const auto& stmt : block.statements) {
            execute(*stmt);
        }
    } catch (...) {
        environment = previous;
        throw;
    }
    environment = previous;
}

MantraValue Interpreter::evaluateBinary(const BinaryExprNode& node) {
    MantraValue left = evaluate(*node.left);
    MantraValue right = evaluate(*node.right);

    switch (node.op) {
        case TokenType::OP_PLUS:
            if (left.type == ValueType::Number && right.type == ValueType::Number) {
                return MantraValue::number(left.number_value + right.number_value);
            }
            return MantraValue::string(left.toString() + right.toString());
        case TokenType::OP_MINUS:
            if (left.type == ValueType::Number && right.type == ValueType::Number) {
                return MantraValue::number(left.number_value - right.number_value);
            }
            runtimeError("'-' के लिए संख्या अपेक्षित है", node);
            return MantraValue::nullValue();
        case TokenType::OP_MULTIPLY:
            if (left.type == ValueType::Number && right.type == ValueType::Number) {
                return MantraValue::number(left.number_value * right.number_value);
            }
            runtimeError("'*' के लिए संख्या अपेक्षित है", node);
            return MantraValue::nullValue();
        case TokenType::OP_DIVIDE:
            if (left.type == ValueType::Number && right.type == ValueType::Number) {
                if (std::abs(right.number_value) < 1e-12) {
                    runtimeError("शून्य से भाग नहीं हो सकता", node);
                    return MantraValue::nullValue();
                }
                return MantraValue::number(left.number_value / right.number_value);
            }
            runtimeError("'/' के लिए संख्या अपेक्षित है", node);
            return MantraValue::nullValue();
        case TokenType::OP_EQUAL:
            return MantraValue::boolean(valuesEqual(left, right));
        case TokenType::OP_NOT_EQUAL:
            return MantraValue::boolean(!valuesEqual(left, right));
        case TokenType::OP_LESS:
            if (left.type == ValueType::Number && right.type == ValueType::Number) {
                return MantraValue::boolean(left.number_value < right.number_value);
            }
            runtimeError("'<' के लिए संख्या अपेक्षित है", node);
            return MantraValue::nullValue();
        case TokenType::OP_LESS_EQUAL:
            if (left.type == ValueType::Number && right.type == ValueType::Number) {
                return MantraValue::boolean(left.number_value <= right.number_value);
            }
            runtimeError("'<=' के लिए संख्या अपेक्षित है", node);
            return MantraValue::nullValue();
        case TokenType::OP_GREATER:
            if (left.type == ValueType::Number && right.type == ValueType::Number) {
                return MantraValue::boolean(left.number_value > right.number_value);
            }
            runtimeError("'>' के लिए संख्या अपेक्षित है", node);
            return MantraValue::nullValue();
        case TokenType::OP_GREATER_EQUAL:
            if (left.type == ValueType::Number && right.type == ValueType::Number) {
                return MantraValue::boolean(left.number_value >= right.number_value);
            }
            runtimeError("'>=' के लिए संख्या अपेक्षित है", node);
            return MantraValue::nullValue();
        default:
            runtimeError("अमान्य ऑपरेटर", node);
            return MantraValue::nullValue();
    }
}

MantraValue Interpreter::evaluateUnary(const UnaryExprNode& node) {
    MantraValue right = evaluate(*node.operand);

    switch (node.op) {
        case TokenType::OP_MINUS:
            if (right.type == ValueType::Number) {
                return MantraValue::number(-right.number_value);
            }
            runtimeError("'-' के लिए संख्या अपेक्षित है", node);
            return MantraValue::nullValue();
        case TokenType::OP_NOT:
            return MantraValue::boolean(!isTruthy(right));
        default:
            runtimeError("अमान्य यूनरी ऑपरेटर", node);
            return MantraValue::nullValue();
    }
}

MantraValue Interpreter::evaluateCall(const CallExprNode& node) {
    MantraValue callee = evaluate(*node.callee);
    if (callee.type != ValueType::Function || !callee.function) {
        runtimeError("कॉल करने के लिए फ़ंक्शन चाहिए", node);
        return MantraValue::nullValue();
    }

    std::vector<MantraValue> args;
    args.reserve(node.arguments.size());
    for (const auto& arg : node.arguments) {
        args.push_back(evaluate(*arg));
    }

    if (callee.function->is_native) {
        return callee.function->native(args);
    }

    if (args.size() != callee.function->params.size()) {
        runtimeError("आर्ग्युमेंट की संख्या मेल नहीं खाती", node);
        return MantraValue::nullValue();
    }

    auto call_env = std::make_shared<Environment>(callee.function->closure);
    for (size_t i = 0; i < args.size(); ++i) {
        call_env->define(callee.function->params[i], args[i]);
    }

    try {
        executeBlock(*callee.function->body, call_env);
    } catch (const ReturnException& ret) {
        return ret.value();
    }

    return MantraValue::nullValue();
}

bool Interpreter::isTruthy(const MantraValue& value) const {
    switch (value.type) {
        case ValueType::Boolean:
            return value.bool_value;
        case ValueType::Null:
            return false;
        case ValueType::Number:
            return std::abs(value.number_value) > 1e-12;
        case ValueType::String:
            return !value.string_value.empty();
        case ValueType::Function:
            return true;
        default:
            return false;
    }
}

bool Interpreter::valuesEqual(const MantraValue& left, const MantraValue& right) const {
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
        default:
            return false;
    }
}

void Interpreter::registerStdlib() {
    auto print_func = std::make_shared<FunctionValue>();
    print_func->is_native = true;
    print_func->name = "print";
    print_func->native = stdlib::builtinPrint;
    globals->define("print", MantraValue::functionValue(print_func));
    globals->define("dikhao", MantraValue::functionValue(print_func));

    auto input_func = std::make_shared<FunctionValue>();
    input_func->is_native = true;
    input_func->name = "input";
    input_func->native = stdlib::builtinInput;
    globals->define("input", MantraValue::functionValue(input_func));
    globals->define("lo", MantraValue::functionValue(input_func));

    auto read_func = std::make_shared<FunctionValue>();
    read_func->is_native = true;
    read_func->name = "read_file";
    read_func->native = stdlib::builtinReadFile;
    globals->define("read_file", MantraValue::functionValue(read_func));
    globals->define("file_padho", MantraValue::functionValue(read_func));

    auto write_func = std::make_shared<FunctionValue>();
    write_func->is_native = true;
    write_func->name = "write_file";
    write_func->native = stdlib::builtinWriteFile;
    globals->define("write_file", MantraValue::functionValue(write_func));
    globals->define("file_likho", MantraValue::functionValue(write_func));
}

void Interpreter::runtimeError(const std::string& message, const MantraNode& node) const {
    ErrorHandler::throwError(ErrorType::RUNTIME_ERROR, message, node.line, node.column, 1);
}

} // namespace mantra
