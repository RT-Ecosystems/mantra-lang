#include "typesystem.h"
#include <sstream>
#include <algorithm>

namespace mantra {
namespace core {

// Type implementation
Type::Type(TypeKind kind) : kind_(kind) {}

bool Type::isPrimitive() const {
    return kind_ == TypeKind::Int || kind_ == TypeKind::Float ||
           kind_ == TypeKind::Bool || kind_ == TypeKind::String ||
           kind_ == TypeKind::Null || kind_ == TypeKind::Void;
}

bool Type::isComposite() const {
    return kind_ == TypeKind::Array || kind_ == TypeKind::Function ||
           kind_ == TypeKind::Object;
}

std::string Type::toString() const {
    switch (kind_) {
        case TypeKind::Int:
            return "int";
        case TypeKind::Float:
            return "float";
        case TypeKind::Bool:
            return "bool";
        case TypeKind::String:
            return "string";
        case TypeKind::Null:
            return "null";
        case TypeKind::Void:
            return "void";
        case TypeKind::Array:
            return element_type_ ? element_type_->toString() + "[]" : "unknown[]";
        case TypeKind::Function: {
            std::ostringstream oss;
            oss << "(";
            for (size_t i = 0; i < param_types_.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << param_types_[i]->toString();
            }
            oss << ") -> " << (return_type_ ? return_type_->toString() : "void");
            return oss.str();
        }
        case TypeKind::Object:
            return "object";
        case TypeKind::Any:
            return "any";
        case TypeKind::Unknown:
            return "unknown";
        default:
            return "?";
    }
}

std::string Type::toDebugString() const {
    std::ostringstream oss;
    oss << "Type{kind=" << toString();
    if (isArray() && element_type_) {
        oss << ", element=" << element_type_->toDebugString();
    }
    if (isFunction()) {
        oss << ", arity=" << param_types_.size();
    }
    oss << "}";
    return oss.str();
}

bool Type::equals(const Type& other) const {
    if (kind_ != other.kind_) return false;
    
    if (isArray()) {
        if (!element_type_ || !other.element_type_) {
            return element_type_ == other.element_type_;
        }
        return element_type_->equals(*other.element_type_);
    }
    
    if (isFunction()) {
        if (param_types_.size() != other.param_types_.size()) return false;
        for (size_t i = 0; i < param_types_.size(); ++i) {
            if (!param_types_[i]->equals(*other.param_types_[i])) return false;
        }
        if (!return_type_ || !other.return_type_) {
            return return_type_ == other.return_type_;
        }
        return return_type_->equals(*other.return_type_);
    }
    
    return true;
}

bool Type::isCompatibleWith(const Type& other) const {
    // Same type is always compatible
    if (equals(other)) return true;
    
    // Any type is compatible with anything
    if (kind_ == TypeKind::Any || other.kind_ == TypeKind::Any) return true;
    
    // Null is compatible with object and array types
    if (kind_ == TypeKind::Null && (other.isArray() || other.isObject())) return true;
    
    // Numeric type compatibility
    if ((kind_ == TypeKind::Int || kind_ == TypeKind::Float) &&
        (other.kind_ == TypeKind::Int || other.kind_ == TypeKind::Float)) {
        return true;  // Allow mixed numeric operations
    }
    
    return false;
}

std::shared_ptr<Type> Type::array(std::shared_ptr<Type> elementType) {
    auto arrType = std::make_shared<Type>(TypeKind::Array);
    arrType->element_type_ = elementType;
    return arrType;
}

std::shared_ptr<Type> Type::function(
    const std::vector<std::shared_ptr<Type>>& paramTypes,
    std::shared_ptr<Type> returnType) {
    auto funcType = std::make_shared<Type>(TypeKind::Function);
    funcType->param_types_ = paramTypes;
    funcType->return_type_ = returnType;
    return funcType;
}

std::shared_ptr<Type> Type::object(
    const std::unordered_map<std::string, std::shared_ptr<Type>>& fields) {
    auto objType = std::make_shared<Type>(TypeKind::Object);
    objType->fields_ = fields;
    return objType;
}

std::shared_ptr<Type> Type::commonType(const Type& a, const Type& b) {
    if (a.equals(b)) return std::make_shared<Type>(a);
    
    // Int + Float = Float
    if ((a.kind() == TypeKind::Int && b.kind() == TypeKind::Float) ||
        (a.kind() == TypeKind::Float && b.kind() == TypeKind::Int)) {
        return types::makeFloat();
    }
    
    // Any other mixed types default to Any
    return types::makeAny();
}

bool Type::canImplicitlyConvertTo(const Type& target) const {
    if (equals(target)) return true;
    
    // Int can convert to Float
    if (kind_ == TypeKind::Int && target.kind() == TypeKind::Float) return true;
    
    // Null can convert to array/object
    if (kind_ == TypeKind::Null && 
        (target.kind() == TypeKind::Array || target.kind() == TypeKind::Object)) {
        return true;
    }
    
    return false;
}

// FunctionSignature implementation
FunctionSignature::FunctionSignature(
    const std::string& name,
    const std::vector<std::shared_ptr<Type>>& paramTypes,
    std::shared_ptr<Type> returnType)
    : name_(name), param_types_(paramTypes), return_type_(returnType),
      is_variadic_(false) {}

std::string FunctionSignature::toString() const {
    std::ostringstream oss;
    oss << name_ << "(";
    for (size_t i = 0; i < param_types_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << param_types_[i]->toString();
    }
    if (is_variadic_) oss << ", ...";
    oss << ") -> " << (return_type_ ? return_type_->toString() : "void");
    return oss.str();
}

// TypeEnvironment implementation
TypeEnvironment::TypeEnvironment() {
    // Start with global scope
    symbol_stack_.emplace_back();
}

TypeEnvironment::~TypeEnvironment() {}

void TypeEnvironment::pushScope() {
    symbol_stack_.emplace_back();
}

void TypeEnvironment::popScope() {
    if (symbol_stack_.size() > 1) {
        symbol_stack_.pop_back();
    }
}

bool TypeEnvironment::defineSymbol(
    const std::string& name,
    std::shared_ptr<Type> type,
    bool isConstant,
    int line,
    int col) {
    if (symbol_stack_.empty()) return false;
    
    auto& current = symbol_stack_.back();
    if (current.find(name) != current.end()) {
        return false;  // Symbol already defined in current scope
    }
    
    current[name] = Symbol(name, type, isConstant, !isConstant, line, col);
    return true;
}

bool TypeEnvironment::defineFunction(
    const std::string& name,
    const FunctionSignature& signature) {
    if (functions_.find(name) != functions_.end()) {
        return false;  // Function already defined
    }
    functions_[name] = signature;
    return true;
}

const Symbol* TypeEnvironment::lookupSymbol(const std::string& name) const {
    // Search from innermost to outermost scope
    for (auto it = symbol_stack_.rbegin(); it != symbol_stack_.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return &found->second;
        }
    }
    return nullptr;
}

const FunctionSignature* TypeEnvironment::lookupFunction(const std::string& name) const {
    auto found = functions_.find(name);
    if (found != functions_.end()) {
        return &found->second;
    }
    return nullptr;
}

bool TypeEnvironment::isDefined(const std::string& name) const {
    return lookupSymbol(name) != nullptr;
}

const std::unordered_map<std::string, Symbol>& TypeEnvironment::currentScope() const {
    static const std::unordered_map<std::string, Symbol> empty;
    return symbol_stack_.empty() ? empty : symbol_stack_.back();
}

// TypeChecker implementation
TypeChecker::TypeChecker() : has_error_(false) {}

std::shared_ptr<Type> TypeChecker::inferType(const MantraNode& node) {
    // Placeholder for type inference - will be implemented with AST integration
    return types::makeAny();
}

bool TypeChecker::isValidAssignment(
    const Type& targetType,
    const Type& sourceType,
    int line,
    int col) {
    if (targetType.equals(sourceType)) return true;
    if (sourceType.canImplicitlyConvertTo(targetType)) return true;
    
    if (line > 0 && col > 0) {
        std::ostringstream oss;
        oss << "Cannot assign " << sourceType.toString() 
            << " to " << targetType.toString()
            << " at line " << line << ":" << col;
        reportError(oss.str(), line, col);
    }
    return false;
}

bool TypeChecker::isValidFunctionCall(
    const FunctionSignature& sig,
    const std::vector<std::shared_ptr<Type>>& argTypes,
    int line,
    int col) {
    
    if (!sig.isVariadic() && argTypes.size() != sig.arity()) {
        if (line > 0 && col > 0) {
            std::ostringstream oss;
            oss << "Function " << sig.name() << " expects " << sig.arity()
                << " arguments, got " << argTypes.size()
                << " at line " << line << ":" << col;
            reportError(oss.str(), line, col);
        }
        return false;
    }
    
    for (size_t i = 0; i < std::min(argTypes.size(), sig.arity()); ++i) {
        if (!argTypes[i]->isCompatibleWith(*sig.paramTypes()[i])) {
            if (line > 0 && col > 0) {
                std::ostringstream oss;
                oss << "Argument " << (i + 1) << " has wrong type. Expected "
                    << sig.paramTypes()[i]->toString() << ", got "
                    << argTypes[i]->toString()
                    << " at line " << line << ":" << col;
                reportError(oss.str(), line, col);
            }
            return false;
        }
    }
    
    return true;
}

bool TypeChecker::isValidBinaryOp(
    const Type& leftType,
    const Type& rightType,
    const std::string& opName,
    int line,
    int col) {
    
    // Arithmetic operators
    if (opName == "+" || opName == "-" || opName == "*" || opName == "/" || opName == "%") {
        bool left_numeric = leftType.kind() == TypeKind::Int || leftType.kind() == TypeKind::Float;
        bool right_numeric = rightType.kind() == TypeKind::Int || rightType.kind() == TypeKind::Float;
        
        if (left_numeric && right_numeric) return true;
        
        // String concatenation with +
        if (opName == "+" && leftType.kind() == TypeKind::String && rightType.kind() == TypeKind::String) {
            return true;
        }
    }
    
    // Comparison operators
    if (opName == "==" || opName == "!=" || opName == "<" || 
        opName == ">" || opName == "<=" || opName == ">=") {
        return leftType.isCompatibleWith(rightType);
    }
    
    // Logical operators
    if (opName == "&&" || opName == "||") {
        return leftType.kind() == TypeKind::Bool && rightType.kind() == TypeKind::Bool;
    }
    
    return false;
}

bool TypeChecker::isValidUnaryOp(
    const Type& operandType,
    const std::string& opName,
    int line,
    int col) {
    
    if (opName == "-" || opName == "+") {
        return operandType.kind() == TypeKind::Int || operandType.kind() == TypeKind::Float;
    }
    
    if (opName == "!") {
        return operandType.kind() == TypeKind::Bool;
    }
    
    return false;
}

void TypeChecker::reportError(
    const std::string& message,
    int line,
    int col) {
    last_error_ = message;
    has_error_ = true;
}

// Factory functions
namespace types {

std::shared_ptr<Type> makeInt() {
    return std::make_shared<Type>(TypeKind::Int);
}

std::shared_ptr<Type> makeFloat() {
    return std::make_shared<Type>(TypeKind::Float);
}

std::shared_ptr<Type> makeBool() {
    return std::make_shared<Type>(TypeKind::Bool);
}

std::shared_ptr<Type> makeString() {
    return std::make_shared<Type>(TypeKind::String);
}

std::shared_ptr<Type> makeNull() {
    return std::make_shared<Type>(TypeKind::Null);
}

std::shared_ptr<Type> makeVoid() {
    return std::make_shared<Type>(TypeKind::Void);
}

std::shared_ptr<Type> makeAny() {
    return std::make_shared<Type>(TypeKind::Any);
}

std::shared_ptr<Type> makeArray(std::shared_ptr<Type> elementType) {
    return Type::array(elementType);
}

std::shared_ptr<Type> makeFunction(
    const std::vector<std::shared_ptr<Type>>& paramTypes,
    std::shared_ptr<Type> returnType) {
    return Type::function(paramTypes, returnType);
}

std::shared_ptr<Type> makeObject(
    const std::unordered_map<std::string, std::shared_ptr<Type>>& fields) {
    return Type::object(fields);
}

} // namespace types

} // namespace core
} // namespace mantra
