#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace mantra {
namespace core {

// Type kind enumeration for all supported types
enum class TypeKind {
    // Primitive types
    Int,
    Float,
    Bool,
    String,
    Null,
    Void,
    
    // Composite types
    Array,
    Function,
    Object,
    
    // Special
    Any,
    Unknown
};

// Forward declarations
class Type;
class FunctionSignature;
class TypeEnvironment;
class TypeChecker;

// Type information class
class Type {
public:
    explicit Type(TypeKind kind);
    
    // Basic type accessors
    TypeKind kind() const { return kind_; }
    bool isNull() const { return kind_ == TypeKind::Null; }
    bool isVoid() const { return kind_ == TypeKind::Void; }
    bool isPrimitive() const;
    bool isComposite() const;
    bool isArray() const { return kind_ == TypeKind::Array; }
    bool isFunction() const { return kind_ == TypeKind::Function; }
    bool isObject() const { return kind_ == TypeKind::Object; }
    
    // Type string representation
    std::string toString() const;
    std::string toDebugString() const;
    
    // Type comparison
    bool equals(const Type& other) const;
    bool isCompatibleWith(const Type& other) const;
    
    // For array types
    static std::shared_ptr<Type> array(std::shared_ptr<Type> elementType);
    std::shared_ptr<Type> elementType() const { return element_type_; }
    
    // For function types
    static std::shared_ptr<Type> function(
        const std::vector<std::shared_ptr<Type>>& paramTypes,
        std::shared_ptr<Type> returnType
    );
    const std::vector<std::shared_ptr<Type>>& paramTypes() const { return param_types_; }
    std::shared_ptr<Type> returnType() const { return return_type_; }
    
    // For object/map types
    static std::shared_ptr<Type> object(
        const std::unordered_map<std::string, std::shared_ptr<Type>>& fields
    );
    const std::unordered_map<std::string, std::shared_ptr<Type>>& fields() const { return fields_; }
    
    // Type conversions
    static std::shared_ptr<Type> commonType(const Type& a, const Type& b);
    bool canImplicitlyConvertTo(const Type& target) const;
    
private:
    TypeKind kind_;
    std::shared_ptr<Type> element_type_;  // For array types
    std::vector<std::shared_ptr<Type>> param_types_;  // For function types
    std::shared_ptr<Type> return_type_;   // For function types
    std::unordered_map<std::string, std::shared_ptr<Type>> fields_;  // For object types
};

// Function signature information
class FunctionSignature {
public:
    // Default constructor required for unordered_map
    FunctionSignature()
        : name_(), param_types_(), return_type_(nullptr), is_variadic_(false) {}
    
    FunctionSignature(
        const std::string& name,
        const std::vector<std::shared_ptr<Type>>& paramTypes,
        std::shared_ptr<Type> returnType
    );
    
    const std::string& name() const { return name_; }
    const std::vector<std::shared_ptr<Type>>& paramTypes() const { return param_types_; }
    std::shared_ptr<Type> returnType() const { return return_type_; }
    
    size_t arity() const { return param_types_.size(); }
    bool isVariadic() const { return is_variadic_; }
    
    std::string toString() const;
    
    void setVariadic(bool variadic) { is_variadic_ = variadic; }
    
private:
    std::string name_;
    std::vector<std::shared_ptr<Type>> param_types_;
    std::shared_ptr<Type> return_type_;
    bool is_variadic_;
};

// Symbol information for type checking
struct Symbol {
    std::string name;
    std::shared_ptr<Type> type;
    bool isConstant;
    bool isMutable;
    int lineNumber;
    int columnNumber;
    
    // Default constructor required for unordered_map
    Symbol() : name(), type(nullptr), isConstant(false), isMutable(true),
               lineNumber(0), columnNumber(0) {}
    
    Symbol(
        const std::string& name_,
        std::shared_ptr<Type> type_,
        bool isConstant_ = false,
        bool isMutable_ = true,
        int line = 0,
        int col = 0
    ) : name(name_), type(type_), isConstant(isConstant_), 
        isMutable(isMutable_), lineNumber(line), columnNumber(col) {}
};

// Type environment for managing scopes
class TypeEnvironment {
public:
    TypeEnvironment();
    ~TypeEnvironment();
    
    // Scope management
    void pushScope();
    void popScope();
    
    // Symbol registration
    bool defineSymbol(
        const std::string& name,
        std::shared_ptr<Type> type,
        bool isConstant = false,
        int line = 0,
        int col = 0
    );
    
    bool defineFunction(
        const std::string& name,
        const FunctionSignature& signature
    );
    
    // Symbol lookup
    const Symbol* lookupSymbol(const std::string& name) const;
    const FunctionSignature* lookupFunction(const std::string& name) const;
    
    // Check if symbol exists
    bool isDefined(const std::string& name) const;
    
    // Get all symbols in current scope
    const std::unordered_map<std::string, Symbol>& currentScope() const;
    
private:
    std::vector<std::unordered_map<std::string, Symbol>> symbol_stack_;
    std::unordered_map<std::string, FunctionSignature> functions_;
};

// Type checker for semantic analysis
class TypeChecker {
public:
    TypeChecker();
    
    // Type inference
    std::shared_ptr<Type> inferType(const class MantraNode& node);
    
    // Type validation
    bool isValidAssignment(
        const Type& targetType,
        const Type& sourceType,
        int line = 0,
        int col = 0
    );
    
    bool isValidFunctionCall(
        const FunctionSignature& sig,
        const std::vector<std::shared_ptr<Type>>& argTypes,
        int line = 0,
        int col = 0
    );
    
    bool isValidBinaryOp(
        const Type& leftType,
        const Type& rightType,
        const std::string& opName,
        int line = 0,
        int col = 0
    );
    
    bool isValidUnaryOp(
        const Type& operandType,
        const std::string& opName,
        int line = 0,
        int col = 0
    );
    
    // Environment management
    TypeEnvironment& environment() { return env_; }
    const TypeEnvironment& environment() const { return env_; }
    
    // Diagnostic support
    std::string getLastError() const { return last_error_; }
    bool hasErrors() const { return has_error_; }
    
protected:
    TypeEnvironment env_;
    std::string last_error_;
    bool has_error_;
    
    void reportError(
        const std::string& message,
        int line = 0,
        int col = 0
    );
};

// Factory functions for common types
namespace types {
    std::shared_ptr<Type> makeInt();
    std::shared_ptr<Type> makeFloat();
    std::shared_ptr<Type> makeBool();
    std::shared_ptr<Type> makeString();
    std::shared_ptr<Type> makeNull();
    std::shared_ptr<Type> makeVoid();
    std::shared_ptr<Type> makeAny();
    std::shared_ptr<Type> makeArray(std::shared_ptr<Type> elementType);
    std::shared_ptr<Type> makeFunction(
        const std::vector<std::shared_ptr<Type>>& paramTypes,
        std::shared_ptr<Type> returnType
    );
    std::shared_ptr<Type> makeObject(
        const std::unordered_map<std::string, std::shared_ptr<Type>>& fields
    );
}

} // namespace core
} // namespace mantra
