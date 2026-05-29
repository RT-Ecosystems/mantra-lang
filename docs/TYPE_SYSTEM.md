# Mantra Type System Documentation

## Phase 1: Static Type System

### Overview
The Static Type System is the foundation for type-safe programming in Mantra. It provides:
- Primitive and composite types
- Type inference and compatibility checking
- Implicit type conversions
- Symbol management with scope tracking
- Function signature handling
- Diagnostic support with source locations

### Architecture

#### Core Components

**1. Type Kinds (`TypeKind` enum)**
- Primitives: `Int`, `Float`, `Bool`, `String`, `Null`, `Void`
- Composites: `Array`, `Function`, `Object`
- Special: `Any`, `Unknown`

**2. Type Class**
Properties:
- Type kind identification
- String representation
- Type equality checking
- Compatibility checking
- Implicit conversion support
- Element type access (for arrays)
- Parameter/return types (for functions)
- Field access (for objects)

Methods:
```cpp
bool equals(const Type& other) const;
bool isCompatibleWith(const Type& other) const;
bool canImplicitlyConvertTo(const Type& target) const;
static std::shared_ptr<Type> commonType(const Type& a, const Type& b);
```

**3. FunctionSignature**
Stores function metadata:
- Name
- Parameter types
- Return type
- Variadic flag
- Arity (parameter count)

**4. Symbol Structure**
Represents declared variables/constants:
- Name
- Type
- Constant flag
- Mutability flag
- Source location (line, column)

**5. TypeEnvironment**
Manages symbol scopes and function lookup:
- Scope stack for nested scopes
- Symbol definition and lookup
- Function registration
- Scope push/pop operations

**6. TypeChecker (Base Class)**
Provides validation methods:
- Assignment type checking
- Binary operation validation
- Unary operation validation
- Function call validation
- Type inference from AST nodes

### Type Compatibility Matrix

**Automatic Conversions**
- `Int` → `Float`
- `Int` → `String` (via toString)
- Any type → `Bool` (truthy/falsy)
- `Null` → Any reference type (arrays, objects)

**No Implicit Conversions**
- `Float` → `Int` (loss of precision)
- `Bool` → `Int`
- `String` → `Int`/`Float`

### Scope Management

The type environment maintains a scope stack for proper variable visibility:

```
Global Scope
  ├─ if block scope
  │  └─ then branch scope
  ├─ while loop scope
  └─ function scope
     ├─ parameter scope
     └─ local scope
```

Symbols are looked up from innermost to outermost scope.

### Type Inference Rules

1. **Number Literals**
   - Integer value → `Int`
   - Float value → `Float`

2. **Arrays**
   - Empty array → `Array<Any>`
   - Single element → `Array<T>` where T is element type
   - Mixed types → `Array<CommonType>`

3. **Binary Operations**
   - `Int + Int` → `Int`
   - `Int + Float` → `Float`
   - `Float + Float` → `Float`
   - `String + String` → `String`
   - Comparison ops → `Bool`

4. **Function Calls**
   - Return type from function signature

### Error Handling

The type checker tracks errors with:
- Error message
- Source location (line, column)
- Error type (TYPE_ERROR)

Common type errors:
- `Unknown identifier`: Variable not in scope
- `Type mismatch`: Assignment types incompatible
- `Wrong argument count`: Function arity mismatch
- `Invalid operation`: Operation not supported for type

### Test Coverage

**73 Type System Tests** covering:
1. Type creation and properties
2. Primitive types (int, float, bool, string, null, void)
3. Array types
4. Function types
5. Object types
6. Type compatibility
7. Implicit conversions
8. Common type resolution
9. Function signatures
10. Symbol management
11. Scope handling
12. Type checker validation

See `tests/test_typesystem.cpp` for detailed test cases.

### Usage Example

```cpp
#include "core/typesystem.h"

using namespace mantra::core;

// Create types
auto int_type = types::makeInt();
auto array_type = types::makeArray(types::makeInt());

// Check compatibility
if (int_type->isCompatibleWith(*types::makeFloat())) {
    // Int is compatible with Float
}

// Use type environment
TypeEnvironment env;
env.defineSymbol("x", types::makeInt());
env.pushScope();
env.defineSymbol("y", types::makeString());

auto sym = env.lookupSymbol("x");
if (sym && sym->type->equals(*int_type)) {
    // Symbol x has int type
}

// Function signatures
FunctionSignature add_sig("add", 
    {types::makeInt(), types::makeInt()}, 
    types::makeInt());
env.defineFunction("add", add_sig);
```

### Performance Considerations

- Type instances are shared via `std::shared_ptr`
- Scope lookups are O(n) where n is scope depth
- Type equality checks perform structural comparison
- Symbol tables use unordered_map for O(1) average lookup

### Future Enhancements

1. **Generic Types**: Template-like type parameters
2. **Union Types**: `Int | String` 
3. **Intersection Types**: Multiple trait satisfaction
4. **Type Aliases**: Named type abstractions
5. **Type Inference Improvements**: More sophisticated inference
6. **Type Narrowing**: Refining types in conditional branches

### Integration Points

The type system is designed to be integrated with:
- **Semantic Analyzer**: For program validation
- **Compiler**: For code generation
- **VM**: For runtime type safety
- **REPL**: For interactive type checking
- **Error Recovery**: For better error messages

### Related Files

- `core/typesystem.h/cpp` - Type system implementation
- `tests/test_typesystem.cpp` - Type system tests (73 tests)
- `semantic/semantic_analyzer.h/cpp` - Program analysis
- `core/ast.h` - AST node definitions

### Key Statistics

- **Types Supported**: 11 type kinds
- **Test Cases**: 73 unit tests
- **Conversion Rules**: 4 automatic conversions
- **Error Types**: 8+ error categories
- **Scope Levels**: Unlimited nesting depth

## Phase 1 Completion Status

✅ **COMPLETE** - All 73 type system tests passing

The type system foundation is ready for:
- Phase 2: Integration with semantic analyzer
- Phase 3: Bytecode generation with type info
- Phase 4: Runtime type checking in VM
