#include "interpreter.h"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "../error/error.h"
#include "../stdlib/network.h"
#include "../stdlib/io.h"
#include "../stdlib/math.h"
#include "../stdlib/string.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace mantra {

namespace {

std::string numberToString(double value) {
    std::ostringstream out;
    out << std::setprecision(15) << value;
    std::string text = out.str();
    if (text.find('.') != std::string::npos) {
        while (!text.empty() && text.back() == '0') {
            text.pop_back();
        }
        if (!text.empty() && text.back() == '.') {
            text.pop_back();
        }
    }
    if (text.empty()) {
        return "0";
    }
    return text;
}





std::string valueToString(const MantraValue& value) {
    switch (value.type) {
        case ValueType::Number:
            return numberToString(value.number_value);
        case ValueType::String:
            return value.string_value;
        case ValueType::Boolean:
            return value.bool_value ? "true" : "false";
        case ValueType::Null:
            return "null";
        case ValueType::Function:
            if (value.function) {
                if (!value.function->name.empty()) {
                    return "<function " + value.function->name + ">";
                }
            }
            return "<function>";
        case ValueType::Array: {
            std::ostringstream out;
            out << "[";
            for (size_t i = 0; i < value.array_value.size(); ++i) {
                out << valueToString(value.array_value[i]);
                if (i + 1 < value.array_value.size()) {
                    out << ", ";
                }
            }
            out << "]";
            return out.str();
        }
        case ValueType::Object:
            return "{object}";
        default:
            return "<unknown>";
    }
}

} // namespace

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

MantraValue MantraValue::array(std::vector<MantraValue> elements) {
    MantraValue result;
    result.type = ValueType::Array;
    result.array_value = std::move(elements);
    return result;
}

MantraValue MantraValue::object(std::unordered_map<std::string, MantraValue> properties) {
    MantraValue result;
    result.type = ValueType::Object;
        // object_value disabled
    return result;
}

std::string MantraValue::typeName() const {
    switch (type) {
        case ValueType::Number:
            return "number";
        case ValueType::String:
            return "string";
        case ValueType::Boolean:
            return "boolean";
        case ValueType::Null:
            return "null";
        case ValueType::Function:
            return "function";
        case ValueType::Array:
            return "array";
        case ValueType::Object:
            return "object";
        default:
            return "unknown";
    }
}

std::string MantraValue::toString() const {
    return valueToString(*this);
}

Environment::Environment(std::shared_ptr<Environment> parent_env)
    : parent(std::move(parent_env)) {}

void Environment::define(const std::string& name, const MantraValue& value) {
    values[name] = value;
}

bool Environment::set(const std::string& name, const MantraValue& value) {
    auto it = values.find(name);
    if (it != values.end()) {
        it->second = value;
        return true;
    }

    if (parent) {
        return parent->set(name, value);
    }

    return false;
}

MantraValue Environment::get(const std::string& name) const {
    auto it = values.find(name);
    if (it != values.end()) {
    // object return disabled
    }

    if (parent) {
        return parent->get(name);
    }

    ErrorHandler::printError(ErrorType::UNKNOWN_IDENTIFIER,
                             "Unknown variable '" + name + "'",
                             0, 0);
    throw std::runtime_error("unknown identifier");
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
        case NodeType::ARRAY_LIT: {
            const auto& array_node = static_cast<const ArrayLitNode&>(node);
            std::vector<MantraValue> elements;
            elements.reserve(array_node.elements.size());
            for (const auto& element : array_node.elements) {
                elements.push_back(evaluate(*element));
            }
            return MantraValue::array(std::move(elements));
        }
        case NodeType::INDEX_EXPR:
            return evaluateIndex(static_cast<const IndexExprNode&>(node));
        case NodeType::BINARY_EXPR:
            return evaluateBinary(static_cast<const BinaryExprNode&>(node));
        case NodeType::UNARY_EXPR:
            return evaluateUnary(static_cast<const UnaryExprNode&>(node));
        case NodeType::CALL_EXPR:
            return evaluateCall(static_cast<const CallExprNode&>(node));
        case NodeType::MEMBER_EXPR:
            return evaluateMember(static_cast<const MemberExprNode&>(node));
        case NodeType::ASSIGN_STMT: {
            const auto& assign_node = static_cast<const AssignStmtNode&>(node);
            MantraValue value = evaluate(*assign_node.value);
            if (!environment->set(assign_node.name, value)) {
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
                try {
                    executeBlock(*while_node.body, environment);
                } catch (const ContinueException&) {
                    continue;
                } catch (const BreakException&) {
                    break;
                }
            }
            break;
        }
        case NodeType::FOR_STMT: {
            const auto& for_node = static_cast<const ForStmtNode&>(node);
            MantraValue start_val = evaluate(*for_node.start);
            MantraValue end_val = evaluate(*for_node.end);
            MantraValue step_val = for_node.step ? evaluate(*for_node.step)
                                                 : MantraValue::number(0.0);

            if (start_val.type != ValueType::Number || end_val.type != ValueType::Number) {
                runtimeError("for/बारबार में संख्या अपेक्षित है", node);
                return;
            }

            double start = start_val.number_value;
            double end = end_val.number_value;
            double step = step_val.type == ValueType::Number ? step_val.number_value : 0.0;

            if (for_node.step) {
                if (step == 0.0) {
                    runtimeError("step/qadam शून्य नहीं हो सकता", node);
                    return;
                }
            } else {
                step = (start <= end) ? 1.0 : -1.0;
            }

            environment->define(for_node.variable, MantraValue::number(start));

            // MANTRA's `baarbaar i = start se end tak` syntax is inclusive so
            // `se 0 0 tak` runs once instead of skipping the boundary value.
            auto condition = [&](double value) {
                return step > 0 ? value <= end : value >= end;
            };

            for (double i = start; condition(i); i += step) {
                environment->set(for_node.variable, MantraValue::number(i));
                try {
                    executeBlock(*for_node.body, environment);
                } catch (const ContinueException&) {
                    continue;
                } catch (const BreakException&) {
                    break;
                }
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
        case NodeType::BREAK_STMT:
            throw BreakException();
        case NodeType::CONTINUE_STMT:
            throw ContinueException();
        case NodeType::ASSIGN_STMT:
        case NodeType::BINARY_EXPR:
        case NodeType::UNARY_EXPR:
        case NodeType::CALL_EXPR:
        case NodeType::EXPR_STMT:
        case NodeType::ARRAY_LIT:
        case NodeType::INDEX_EXPR:
        case NodeType::NUMBER_LIT:
        case NodeType::STRING_LIT:
        case NodeType::BOOL_LIT:
        case NodeType::NULL_LIT:
        case NodeType::IDENTIFIER:
        case NodeType::MEMBER_EXPR:
            evaluate(node);
            break;
        default:
            runtimeError("अमान्य स्टेटमेंट", node);
            break;
    }
}

void Interpreter::executeBlock(const BlockStmtNode& block,
                              std::shared_ptr<Environment> new_env) {
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
    if (node.op == TokenType::OP_AND) {
        MantraValue left = evaluate(*node.left);
        if (!isTruthy(left)) {
            return MantraValue::boolean(false);
        }
        MantraValue right = evaluate(*node.right);
        return MantraValue::boolean(isTruthy(right));
    }

    if (node.op == TokenType::OP_OR) {
        MantraValue left = evaluate(*node.left);
        if (isTruthy(left)) {
            return MantraValue::boolean(true);
        }
        MantraValue right = evaluate(*node.right);
        return MantraValue::boolean(isTruthy(right));
    }

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
        case TokenType::OP_MODULO:
            if (left.type == ValueType::Number && right.type == ValueType::Number) {
                if (std::abs(right.number_value) < 1e-12) {
                    runtimeError("शून्य से भाग नहीं हो सकता", node);
                    return MantraValue::nullValue();
                }
                return MantraValue::number(std::fmod(left.number_value, right.number_value));
            }
            runtimeError("'%' के लिए संख्या अपेक्षित है", node);
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
        runtimeError("Function '" + callee.function->name + "' expects " +
                     std::to_string(callee.function->params.size()) + " arguments but got " +
                     std::to_string(args.size()), node);
        return MantraValue::nullValue();
    }

    if (call_depth >= kMaxCallDepth) {
        runtimeError("Maximum call depth exceeded", node);
        return MantraValue::nullValue();
    }

    auto call_env = std::make_shared<Environment>(callee.function->closure);
    for (size_t i = 0; i < args.size(); ++i) {
        call_env->define(callee.function->params[i], args[i]);
    }

    ++call_depth;
    struct CallDepthGuard {
        size_t& depth;
        ~CallDepthGuard() { --depth; }
    } guard{call_depth};
    (void)guard;
    try {
        executeBlock(*callee.function->body, call_env);
    } catch (const ReturnException& ret) {
        return ret.value();
    } catch (const BreakException&) {
        runtimeError("break cannot be used here", node);
    } catch (const ContinueException&) {
        runtimeError("continue cannot be used here", node);
    }

    return MantraValue::nullValue();
}

MantraValue Interpreter::evaluateIndex(const IndexExprNode& node) {
    MantraValue target = evaluate(*node.collection);
    MantraValue index_val = evaluate(*node.index);

    if (index_val.type != ValueType::Number) {
        runtimeError("index के लिए संख्या अपेक्षित है", node);
        return MantraValue::nullValue();
    }

    int index = static_cast<int>(index_val.number_value);
    if (target.type == ValueType::Array) {
        if (index < 0 || static_cast<size_t>(index) >= target.array_value.size()) {
            runtimeError("array index सीमा से बाहर है", node);
            return MantraValue::nullValue();
        }
        return target.array_value[static_cast<size_t>(index)];
    }

    if (target.type == ValueType::String) {
        if (index < 0 || static_cast<size_t>(index) >= target.string_value.size()) {
            runtimeError("string index सीमा से बाहर है", node);
            return MantraValue::nullValue();
        }
        return MantraValue::string(std::string(1, target.string_value[static_cast<size_t>(index)]));
    }

    runtimeError("केवल array या string का index लिया जा सकता है", node);
    return MantraValue::nullValue();
}

MantraValue Interpreter::evaluateMember(const MemberExprNode& node) {
    const auto& memberNode = static_cast<const MemberExprNode&>(node);
    MantraValue target = evaluate(*memberNode.object);
    if (target.type != ValueType::Object) {
        runtimeError("dot access केवल object पर चलेगा", node);
        return MantraValue::nullValue();
    }

        // object disabled
        // object disabled
        runtimeError("Unknown property '" + memberNode.property + "'", node);
        return MantraValue::nullValue();
bool Interpreter::isTruthy(const MantraValue& value) const {
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
        case ValueType::Array:
            return !value.array_value.empty();
        case ValueType::Object:
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
        case ValueType::Array:
            if (left.array_value.size() != right.array_value.size()) {
                return false;
            }
            for (size_t i = 0; i < left.array_value.size(); ++i) {
                if (!valuesEqual(left.array_value[i], right.array_value[i])) {
                    return false;
                }
            }
            return true;
        case ValueType::Object:
        // size=0
                return false;
            }
        //             for (const auto& entry : left.object_value) {
        // object disabled
        // object disabled
                    return false;
            return true;
        default:
            return false;
    }
}

void Interpreter::registerStdlib() {
    auto addNative = [&](const std::string& name,
                         const std::function<MantraValue(const std::vector<MantraValue>&)>& fn) {
        auto func_value = std::make_shared<FunctionValue>();
        func_value->is_native = true;
        func_value->name = name;
        func_value->native = fn;
        globals->define(name, MantraValue::functionValue(func_value));
    };

    auto addAliases = [&](const std::vector<std::string>& names,
                          const std::function<MantraValue(const std::vector<MantraValue>&)>& fn) {
        for (const auto& name : names) {
            addNative(name, fn);
        }
    };

    auto defineConstant = [&](const std::string& name, double value) {
        globals->define(name, MantraValue::number(value));
    };

    const std::vector<std::string> print_aliases = {
        "print",
        "println",
        "dikhao",
        "kaado",
        "dakho",
        "dekhao",
        "batavo",
        "dakhav",
        "chupinchu",
        "toro",
        "kaaniku",
        "jadi",
        "nohole",
        "dekhaow",
        "dakhoi",
        "dekhau",
        "darshaya",
        "waatav",
        "dikhay",
        "wekho",
        "nungsi",
        "nangi",
        "dado"
    };

    for (const auto& name : print_aliases) {
        addNative(name, stdlib::builtinPrint);
    }

    addNative("input", stdlib::builtinInput);
    addNative("lo", stdlib::builtinInput);
    addNative("loo", stdlib::builtinInput);

    addNative("lambai", stdlib::builtinLength);
    addNative("length", stdlib::builtinLength);

    addNative("jodo_shabd", stdlib::builtinConcat);
    addNative("concat", stdlib::builtinConcat);

    addNative("sankhya", stdlib::builtinToNumber);
    addNative("toNumber", stdlib::builtinToNumber);

    addNative("shabd", stdlib::builtinToString);
    addNative("toString", stdlib::builtinToString);

    addNative("type", stdlib::builtinType);
    addNative("roke", stdlib::builtinExit);
    addNative("exit", stdlib::builtinExit);
    addNative("saaf", stdlib::builtinClear);
    addNative("clear", stdlib::builtinClear);

    addNative("upper", stdlib::builtinUpper);
    addNative("upar_case", stdlib::builtinUpper);
    addNative("lower", stdlib::builtinLower);
    addNative("neeche_case", stdlib::builtinLower);
    addNative("trim", stdlib::builtinTrim);
    addNative("katao", stdlib::builtinTrim);
    addNative("split", stdlib::builtinSplit);
    addNative("vibhajit", stdlib::builtinSplit);
    addNative("replace", stdlib::builtinReplace);
    addNative("badlo", stdlib::builtinReplace);
    addNative("slice", stdlib::builtinSlice);
    addNative("part", stdlib::builtinSlice);
    addNative("contains", stdlib::builtinContains);
    addNative("shamil", stdlib::builtinContains);
    addNative("startsWith", stdlib::builtinStartsWith);
    addNative("shuru", stdlib::builtinStartsWith);
    addNative("endsWith", stdlib::builtinEndsWith);
    addNative("ant", stdlib::builtinEndsWith);
    addNative("repeat", stdlib::builtinRepeat);
    addNative("dohrao", stdlib::builtinRepeat);

                
    addAliases({"jodo", "koodal", "joran", "jogao"}, stdlib::builtinAdd);
    addAliases({"ghato", "kayal", "tafawut"}, stdlib::builtinSubtract);
    addAliases({"guno", "guna", "perukku"}, stdlib::builtinMultiply);
    addAliases({"bhaago", "vagat", "vani"}, stdlib::builtinDivide);
    addAliases({"baki", "meechi", "seshamu"}, stdlib::builtinRemainder);

    addAliases({"abs", "mutlak", "nirnayaka"}, stdlib::builtinAbs);
    addAliases({"sqrt", "varga", "karan", "vargamul", "mul"}, stdlib::builtinSqrt);
    addAliases({"pow", "shakti", "mattu", "potens"}, stdlib::builtinPow);
    addAliases({"round", "gol", "suzhal"}, stdlib::builtinRound);
    addAliases({"ceil", "upar", "melu"}, stdlib::builtinCeil);
    addAliases({"floor", "neeche", "kizh"}, stdlib::builtinFloor);

    addAliases({"madhyaman", "mean", "average"}, stdlib::builtinMean);
    addAliases({"madhyika", "median"}, stdlib::builtinMedian);
    addAliases({"bahulak", "mode"}, stdlib::builtinMode);
    addAliases({"vistar", "range"}, stdlib::builtinRange);
    addAliases({"pramaan", "std_deviation"}, stdlib::builtinStdDeviation);
    addAliases({"yogfal", "sum"}, stdlib::builtinSum);
    addAliases({"adhiktam", "maximum", "max"}, stdlib::builtinMaximum);
    addAliases({"nyuntam", "minimum", "min"}, stdlib::builtinMinimum);

    addNative("random", stdlib::builtinRandom);
    addNative("pi", stdlib::builtinPi);
    addNative("e", stdlib::builtinE);
    defineConstant("PI", 3.14159265358979);
    defineConstant("E", 2.71828182845905);
    addNative("infinity", stdlib::builtinInfinity);
    addNative("nan", stdlib::builtinNaN);
    addNative("is_infinity", stdlib::builtinIsInfinity);
    addNative("is_nan", stdlib::builtinIsNaN);
}

void Interpreter::runtimeError(const std::string& message, const MantraNode& node) const {
    std::cerr << "[MANTRA Error] Line " << node.line << ": " << message << std::endl;
    throw std::runtime_error(message);
}










































































































































































































} // namespace mantra
