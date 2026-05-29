#include <iostream>
#include <cassert>
#include <memory>
#include "../core/typesystem.h"

using namespace mantra::core;

namespace {

int test_count = 0;
int pass_count = 0;

void assert_test(bool condition, const std::string& test_name) {
    test_count++;
    if (condition) {
        pass_count++;
        std::cout << "✓ " << test_name << std::endl;
    } else {
        std::cout << "✗ " << test_name << std::endl;
    }
}

}

void runTypeSystemTests() {
    test_count = 0;
    pass_count = 0;
    
    std::cout << "=== MANTRA Type System Tests ===" << std::endl;
    
    // Test 1: Type creation
    {
        auto int_type = types::makeInt();
        assert_test(int_type->kind() == TypeKind::Int, "Create int type");
        assert_test(int_type->toString() == "int", "Int type string representation");
    }
    
    // Test 2: All primitive types
    {
        auto float_type = types::makeFloat();
        auto bool_type = types::makeBool();
        auto string_type = types::makeString();
        auto null_type = types::makeNull();
        auto void_type = types::makeVoid();
        
        assert_test(float_type->toString() == "float", "Float type");
        assert_test(bool_type->toString() == "bool", "Bool type");
        assert_test(string_type->toString() == "string", "String type");
        assert_test(null_type->toString() == "null", "Null type");
        assert_test(void_type->toString() == "void", "Void type");
    }
    
    // Test 3: Primitive type checks
    {
        auto int_type = types::makeInt();
        auto func_type = types::makeFunction({types::makeInt()}, types::makeInt());
        
        assert_test(int_type->isPrimitive(), "Int is primitive");
        assert_test(!func_type->isPrimitive(), "Function is not primitive");
        assert_test(func_type->isComposite(), "Function is composite");
    }
    
    // Test 4: Type equality
    {
        auto int1 = types::makeInt();
        auto int2 = types::makeInt();
        auto float_type = types::makeFloat();
        
        assert_test(int1->equals(*int2), "Same types are equal");
        assert_test(!int1->equals(*float_type), "Different types are not equal");
    }
    
    // Test 5: Array types
    {
        auto int_array = types::makeArray(types::makeInt());
        assert_test(int_array->isArray(), "Created array type");
        assert_test(int_array->toString() == "int[]", "Array type string representation");
        assert_test(int_array->elementType()->equals(*types::makeInt()), "Array element type");
    }
    
    // Test 6: Function types
    {
        auto params = std::vector<std::shared_ptr<Type>>{
            types::makeInt(),
            types::makeInt()
        };
        auto func_type = types::makeFunction(params, types::makeInt());
        
        assert_test(func_type->isFunction(), "Created function type");
        assert_test(func_type->paramTypes().size() == 2, "Function arity");
        assert_test(func_type->returnType()->equals(*types::makeInt()), "Function return type");
    }
    
    // Test 7: Type compatibility
    {
        auto int_type = types::makeInt();
        auto float_type = types::makeFloat();
        auto string_type = types::makeString();
        
        assert_test(int_type->isCompatibleWith(*int_type), "Type compatible with itself");
        assert_test(int_type->isCompatibleWith(*float_type), "Int compatible with float");
        assert_test(!int_type->isCompatibleWith(*string_type), "Int not compatible with string");
    }
    
    // Test 8: Implicit conversions
    {
        auto int_type = types::makeInt();
        auto float_type = types::makeFloat();
        auto string_type = types::makeString();
        
        assert_test(int_type->canImplicitlyConvertTo(*float_type), "Int can convert to float");
        assert_test(!float_type->canImplicitlyConvertTo(*int_type), "Float cannot convert to int");
        assert_test(!int_type->canImplicitlyConvertTo(*string_type), "Int cannot convert to string");
    }
    
    // Test 9: Common type
    {
        auto int_type = types::makeInt();
        auto float_type = types::makeFloat();
        auto common = Type::commonType(*int_type, *float_type);
        
        assert_test(common->kind() == TypeKind::Float, "Common type of int and float is float");
    }
    
    // Test 10: Function signature creation
    {
        auto params = std::vector<std::shared_ptr<Type>>{
            types::makeInt(),
            types::makeString()
        };
        FunctionSignature sig("myFunc", params, types::makeInt());
        
        assert_test(sig.name() == "myFunc", "Function signature name");
        assert_test(sig.arity() == 2, "Function signature arity");
        assert_test(sig.returnType()->equals(*types::makeInt()), "Function signature return type");
    }
    
    // Test 11: Function signature variadic
    {
        FunctionSignature sig("varFunc", {}, types::makeVoid());
        sig.setVariadic(true);
        
        assert_test(sig.isVariadic(), "Set function as variadic");
    }
    
    // Test 12: TypeEnvironment - basic operations
    {
        TypeEnvironment env;
        assert_test(env.defineSymbol("x", types::makeInt()), "Define symbol");
        assert_test(!env.defineSymbol("x", types::makeInt()), "Cannot redefine symbol in same scope");
        assert_test(env.isDefined("x"), "Symbol is defined");
        assert_test(!env.isDefined("y"), "Undefined symbol");
    }
    
    // Test 13: TypeEnvironment - symbol lookup
    {
        TypeEnvironment env;
        env.defineSymbol("x", types::makeInt(), false, 1, 5);
        
        const Symbol* sym = env.lookupSymbol("x");
        assert_test(sym != nullptr, "Symbol lookup returns valid pointer");
        assert_test(sym->name == "x", "Symbol has correct name");
        assert_test(sym->type->kind() == TypeKind::Int, "Symbol has correct type");
        assert_test(sym->lineNumber == 1 && sym->columnNumber == 5, "Symbol has location info");
    }
    
    // Test 14: TypeEnvironment - scope management
    {
        TypeEnvironment env;
        env.defineSymbol("x", types::makeInt());
        env.pushScope();
        env.defineSymbol("y", types::makeString());
        
        assert_test(env.isDefined("x"), "Symbol from outer scope is visible");
        assert_test(env.isDefined("y"), "Symbol in inner scope is visible");
        
        env.popScope();
        assert_test(env.isDefined("x"), "Outer scope symbol still visible");
        assert_test(!env.isDefined("y"), "Inner scope symbol no longer visible");
    }
    
    // Test 15: TypeEnvironment - nested scopes
    {
        TypeEnvironment env;
        env.defineSymbol("a", types::makeInt());
        env.pushScope();
        env.defineSymbol("b", types::makeString());
        env.pushScope();
        env.defineSymbol("c", types::makeBool());
        
        assert_test(env.isDefined("a"), "Level 1 scope visible");
        assert_test(env.isDefined("b"), "Level 2 scope visible");
        assert_test(env.isDefined("c"), "Level 3 scope visible");
        
        env.popScope();
        assert_test(!env.isDefined("c"), "Level 3 scope removed");
        assert_test(env.isDefined("b"), "Level 2 scope still visible");
    }
    
    // Test 16: TypeEnvironment - function registration
    {
        TypeEnvironment env;
        FunctionSignature sig("add", {types::makeInt(), types::makeInt()}, types::makeInt());
        assert_test(env.defineFunction("add", sig), "Register function");
        
        const FunctionSignature* looked_up = env.lookupFunction("add");
        assert_test(looked_up != nullptr, "Function lookup works");
        assert_test(looked_up->arity() == 2, "Looked up function has correct arity");
    }
    
    // Test 17: TypeChecker - valid assignment
    {
        TypeChecker checker;
        auto int_type = types::makeInt();
        auto float_type = types::makeFloat();
        
        assert_test(checker.isValidAssignment(*int_type, *int_type), "Same type assignment valid");
        assert_test(checker.isValidAssignment(*float_type, *types::makeInt()), "Int to float assignment valid");
        assert_test(!checker.isValidAssignment(*int_type, *types::makeString()), "String to int assignment invalid");
    }
    
    // Test 18: TypeChecker - valid binary operations
    {
        TypeChecker checker;
        auto int_type = types::makeInt();
        auto float_type = types::makeFloat();
        auto string_type = types::makeString();
        auto bool_type = types::makeBool();
        
        assert_test(checker.isValidBinaryOp(*int_type, *int_type, "+"), "Int + Int valid");
        assert_test(checker.isValidBinaryOp(*int_type, *float_type, "+"), "Int + Float valid");
        assert_test(checker.isValidBinaryOp(*string_type, *string_type, "+"), "String + String valid");
        assert_test(checker.isValidBinaryOp(*int_type, *int_type, "=="), "Int == Int valid");
        assert_test(checker.isValidBinaryOp(*bool_type, *bool_type, "&&"), "Bool && Bool valid");
        assert_test(!checker.isValidBinaryOp(*int_type, *bool_type, "+"), "Int + Bool invalid");
    }
    
    // Test 19: TypeChecker - valid unary operations
    {
        TypeChecker checker;
        auto int_type = types::makeInt();
        auto float_type = types::makeFloat();
        auto bool_type = types::makeBool();
        auto string_type = types::makeString();
        
        assert_test(checker.isValidUnaryOp(*int_type, "-"), "Unary minus on int valid");
        assert_test(checker.isValidUnaryOp(*float_type, "-"), "Unary minus on float valid");
        assert_test(checker.isValidUnaryOp(*bool_type, "!"), "Unary not on bool valid");
        assert_test(!checker.isValidUnaryOp(*string_type, "-"), "Unary minus on string invalid");
    }
    
    // Test 20: TypeChecker - valid function calls
    {
        TypeChecker checker;
        FunctionSignature sig("add", {types::makeInt(), types::makeInt()}, types::makeInt());
        
        std::vector<std::shared_ptr<Type>> args1{types::makeInt(), types::makeInt()};
        assert_test(checker.isValidFunctionCall(sig, args1), "Correct argument count valid");
        
        std::vector<std::shared_ptr<Type>> args2{types::makeInt()};
        assert_test(!checker.isValidFunctionCall(sig, args2), "Wrong argument count invalid");
    }
    
    // Test 21: Type debug string
    {
        auto int_type = types::makeInt();
        auto array_type = types::makeArray(types::makeInt());
        
        std::string debug_str = int_type->toDebugString();
        assert_test(!debug_str.empty(), "Debug string generated");
        assert_test(debug_str.find("int") != std::string::npos, "Debug string contains type");
    }
    
    // Test 22: Type null compatibility
    {
        auto null_type = types::makeNull();
        auto array_type = types::makeArray(types::makeInt());
        auto obj_type = types::makeObject({});
        
        assert_test(null_type->isCompatibleWith(*array_type), "Null compatible with array");
        assert_test(null_type->isCompatibleWith(*obj_type), "Null compatible with object");
    }
    
    // Test 23: Constant symbols
    {
        TypeEnvironment env;
        env.defineSymbol("PI", types::makeFloat(), true);
        
        const Symbol* sym = env.lookupSymbol("PI");
        assert_test(sym != nullptr, "Constant symbol defined");
        assert_test(sym->isConstant, "Symbol marked as constant");
        assert_test(!sym->isMutable, "Constant symbol immutable");
    }
    
    // Test 24: Object type with fields
    {
        std::unordered_map<std::string, std::shared_ptr<Type>> fields;
        fields["x"] = types::makeInt();
        fields["y"] = types::makeInt();
        
        auto obj_type = types::makeObject(fields);
        assert_test(obj_type->isObject(), "Created object type");
        assert_test(obj_type->fields().size() == 2, "Object has correct field count");
    }
    
    std::cout << "\n=== Type System Results ===" << std::endl;
    std::cout << "Passed: " << pass_count << "/" << test_count << std::endl;
}
