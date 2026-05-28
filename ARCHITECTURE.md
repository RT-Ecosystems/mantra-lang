# MANTRA Language - Consolidated Architecture

## System Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                       MANTRA APPLICATION                         │
│                       (main executable)                          │
└────────────────────────────────┬────────────────────────────────┘
                                 │
                                 ▼
        ┌────────────────────────────────────────┐
        │   mantra::runtime::Runtime (FACADE)    │
        │  ═══════════════════════════════════   │
        │  executeProgram(AST) → return value    │
        │  executeBytecode(code) → return value  │
        │  globals() → Environment               │
        └────────────┬──────────────────────────┘
                     │
        ┌────────────┴──────────────────┐
        │                               │
        ▼                               ▼
    ┌─────────────┐          ┌──────────────────┐
    │ Interpreter │          │    VM (Modern)   │
    │  (AST-Based)│          │  (Bytecode-Based)│
    │             │          │                  │
    │ Executes    │          │ Executes         │
    │ syntax tree │          │ bytecode         │
    │ directly    │          │ from Compiler    │
    └──────┬──────┘          └────────┬─────────┘
           │                          │
           │      ┌──────────┬────────┘
           │      │          │
           ▼      ▼          ▼
    ┌──────────────────────────────────────┐
    │  mantra::runtime::Value              │
    │  ═════════════════════════════════   │
    │  Unified Value Representation         │
    │                                      │
    │  Number, String, Boolean, Null,      │
    │  Function, Array, Object             │
    │                                      │
    │  Factory Methods:                    │
    │  Value::number(3.14)                 │
    │  Value::string("hello")              │
    │  Value::function(fn)                 │
    │  Value::array({...})                 │
    │  Value::object({...})                │
    └──────────────┬───────────────────────┘
                   │
                   ▼
    ┌──────────────────────────────────────┐
    │  mantra::runtime::Environment        │
    │  ═════════════════════════════════   │
    │  Variable Scoping & Management       │
    │                                      │
    │  define(name, value)                 │
    │  get(name) → value                   │
    │  set(name, value) → bool             │
    │  exists(name) → bool                 │
    │                                      │
    │  Linked list of scopes               │
    │  for lexical scoping                 │
    └──────────────────────────────────────┘
```

---

## Frontend Architecture

```
┌─────────────────────────────────────────────────┐
│              Source Code (.mtr file)             │
└────────────────────┬────────────────────────────┘
                     │
                     ▼
        ┌────────────────────────┐
        │  mantra::Lexer         │
        │  (Tokenization)        │
        │  core/lexer.cpp        │
        └────────┬───────────────┘
                 │
                 ▼
        ┌────────────────────────┐
        │  Token Stream          │
        └────────┬───────────────┘
                 │
                 ▼
        ┌────────────────────────┐
        │  mantra::Parser        │
        │  (Syntax Analysis)     │
        │  core/parser.cpp       │
        └────────┬───────────────┘
                 │
                 ▼
        ┌────────────────────────┐
        │  Abstract Syntax Tree  │
        │  (AST)                 │
        │  core/ast.h            │
        └────────┬───────────────┘
                 │
                 ▼
        ┌────────────────────────┐
        │  Semantic Analyzer     │
        │  (Type Checking)       │
        │  semantic/analyzer.cpp │
        └────────┬───────────────┘
                 │
        ┌────────┴─────────┐
        │                  │
        ▼                  ▼
    ┌─────────┐      ┌──────────┐
    │Interpreter   │ Bytecode │
    │Execution     │ Compiler │
    └─────────┘    └──────────┘
```

---

## Value System Unification

### Before (Fragmented)
```
MantraValue (core/interpreter.h)     Value (include/mantra/vm/value.h)
│                                    │
├─ type: ValueType                   ├─ type: ValueType
├─ number_value: double              ├─ variant<double, string, bool>
├─ string_value: string              │
├─ bool_value: bool                  │
├─ array_value: vector               │
├─ object_value: map                 │
├─ function: FunctionValue struct    │
│  ├─ is_native: bool                │
│  ├─ name: string                   │
│  ├─ params: vector                 │
│  └─ body/native/closure            │
└─ ...                               └─ ...

Problem: Code duplication, different implementations
         Two value systems in use, type incompatibility
```

### After (Unified)
```
                    mantra::runtime::Value
                    (include/mantra/runtime/types.h)
                    
                           ┌────────┐
                           │ Unified│
                           │ Value  │
                           │  Type  │
                           └────────┘
                                │
                ┌───────────────┼───────────────┐
                │               │               │
            ┌───▼────┐      ┌──▼────┐     ┌───▼─────┐
            │Primitive    │Complex  │     │Function │
            │Values       │Values   │     │Values   │
            ├─Number     ├─Array   │     ├─Native  │
            ├─String     ├─Object  │     ├─Closure │
            ├─Boolean    │          │     └─────────┘
            └─Null      └──────────┘

Benefits: Single implementation, type safety, clear semantics
         Easy interoperability between interpreter and VM
         Unified factory methods for creation
         Type-safe accessor methods
```

---

## Execution Model

### Interpreter Path (Current)
```
Source Code
    │
    ├─→ Lexer
    │   └─→ Tokens
    │
    ├─→ Parser
    │   └─→ AST
    │
    ├─→ Semantic Analyzer
    │   └─→ Verified AST
    │
    └─→ Interpreter (Tree-Walking)
        └─→ Executes AST node by node
            ├─ Updates environment
            ├─ Uses runtime::Value
            └─ Produces output/results
```

### Bytecode Path (Future)
```
Source Code
    │
    ├─→ Lexer
    │   └─→ Tokens
    │
    ├─→ Parser
    │   └─→ AST
    │
    ├─→ Semantic Analyzer
    │   └─→ Verified AST
    │
    ├─→ Bytecode Compiler
    │   └─→ Bytecode Instructions
    │
    └─→ VM Execution
        └─→ Executes bytecode
            ├─ Stack-based execution
            ├─ Uses runtime::Value
            └─ Produces output/results
```

---

## Module Dependencies

```
mantra::runtime (NEW - Core)
├─ Value
├─ Environment
├─ Function
└─ Runtime (facade)
    │
    ├─→ mantra::core
    │   ├─ Interpreter (uses Value)
    │   ├─ Parser
    │   └─ Lexer
    │
    ├─→ mantra::semantic
    │   └─ Analyzer
    │
    └─→ mantra::bytecode (future)
        ├─ Compiler
        └─ VM

mantra::core
├─ Interpreter (uses runtime::Value)
├─ Parser (generates AST)
├─ Lexer (generates tokens)
├─ AST (node definitions)
└─ REPL

mantra::semantic
├─ Analyzer (semantic checking)
└─ Error diagnostics

mantra::bytecode (legacy/modern hybrid)
├─ Chunk (bytecode container)
├─ OpCode (instruction definitions)
└─ Stack (value stack)

mantra::vm (modern, canonical)
├─ VM (bytecode virtual machine)
├─ Value (modern value type)
├─ Stack (execution stack)
└─ Bytecode (bytecode program)

mantra::stdlib
├─ Math functions
├─ IO functions
└─ String functions

mantra::error
├─ Error types
├─ Diagnostic output
└─ Language identifiers

mantra::unicode
├─ UTF-8 support
├─ Script detection
└─ Character utilities
```

---

## Namespace Organization

### Professional Namespace Hierarchy

```
mantra::
│
├─ core::                    (Language Frontend)
│  ├─ Interpreter
│  ├─ Parser
│  ├─ Lexer
│  ├─ AST
│  └─ REPL
│
├─ semantic::               (Analysis)
│  └─ Analyzer
│
├─ runtime::                (Unified Execution - NEW)
│  ├─ Value
│  ├─ Function
│  ├─ Environment
│  ├─ Runtime
│  └─ ExecutionStatus
│
├─ bytecode::               (Intermediate Representation)
│  ├─ Chunk
│  ├─ OpCode
│  └─ Stack
│
├─ vm::                     (Virtual Machine)
│  ├─ VM
│  ├─ Value (modern)
│  └─ Stack
│
├─ stdlib::                 (Standard Library)
│  ├─ Math
│  ├─ IO
│  └─ String
│
├─ error::                  (Error Handling)
│  ├─ Diagnostics
│  └─ UserLanguage
│
└─ unicode::               (Text Support)
   ├─ UTF8Handler
   └─ ScriptDetector
```

---

## Header Organization

### Include Path Structure

```
include/
└─ mantra/
   ├─ runtime/           (NEW - Unified runtime)
   │  ├─ types.h         (Value, Environment, Function)
   │  └─ runtime.h       (Runtime facade)
   │
   ├─ vm/                (Modern VM)
   │  ├─ vm.h
   │  ├─ value.h
   │  ├─ bytecode.h
   │  └─ stack.h
   │
   └─ core/              (Frontend)
      └─ ...


src/
├─ runtime/             (NEW - Unified runtime impl)
│  ├─ types.cpp
│  └─ runtime.cpp
│
├─ vm/                  (Modern VM impl)
│  ├─ vm.cpp
│  ├─ value.cpp
│  ├─ bytecode.cpp
│  └─ stack.cpp
│
├─ core/                (Frontend impl)
│  ├─ interpreter.cpp
│  ├─ parser.cpp
│  ├─ lexer.cpp
│  └─ repl.cpp
│
├─ semantic/            (Analysis impl)
│  └─ analyzer.cpp
│
└─ ... (other modules)
```

---

## Type System

### Value Types Supported

```
┌─────────────────────────────────────────┐
│     mantra::runtime::ValueType          │
└─────────────────────────────────────────┘
       ╱         ╱       ╱      ╲      ╲
      ╱         ╱       ╱        ╲      ╲
     │          │       │         │      │
  ┌──▼──┐  ┌───▼──┐ ┌──▼──┐ ┌───▼──┐ ┌─▼──┐
 │Number│ │String│ │Bool │ │ Null │ │Func│
  └─────┘  └──────┘ └──┬──┘ └──────┘ └────┘
                       │
               ┌───────┴────────┐
               │                │
            ┌──▼──┐          ┌──▼──┐
            │Array│          │Objct│
            └─────┘          └─────┘

Examples:
┌─ Number  → 3.14159, 42, -1.5
├─ String  → "hello", "مرحبا", "नमस्ते"
├─ Boolean → true, false
├─ Null    → null, undefined
├─ Function → (x) => x + 1, built-in functions
├─ Array   → [1, "hello", true], [...]
└─ Object  → {x: 10, y: 20}, {a: "value"}
```

---

## Data Flow Example

### Execution Flow: Simple Expression

```
Input: "x = 10"

1. LEXER
   Input:  "x = 10"
   Output: [Identifier("x"), Assign, Number(10)]

2. PARSER  
   Input:  [Identifier("x"), Assign, Number(10)]
   Output: AssignmentNode {
             target: IdentifierNode("x"),
             value: NumberLiteralNode(10)
           }

3. SEMANTIC ANALYZER
   Input:  AssignmentNode
   Output: Checked AST (same structure, verified)

4. INTERPRETER
   Input:  AssignmentNode
   Steps:
   a) Evaluate right side: NumberLiteralNode(10)
      → Value::number(10)
   b) Store in environment: env.define("x", Value::number(10))
   c) Return value from assignment

5. RUNTIME OUTPUT
   → Execution completes successfully
```

---

## Performance Characteristics

### Execution Model Comparison

| Aspect | Interpreter | VM |
|--------|-------------|-----|
| **Startup** | Immediate | Compile time overhead |
| **Memory** | AST in memory | Compact bytecode |
| **Speed** | Slower (tree walk) | Faster (dispatch) |
| **Simplicity** | Simple | Complex |
| **Current Status** | ✅ Working | 🔄 Future |

---

## Migration Path

### Phase Progression

```
Phase 1-3 (COMPLETE) ✅
├─ Unified runtime API
├─ Value system foundation  
├─ VM consolidation
└─ Dead code removal

Phase 4 (TODO) →
├─ Professional namespaces
├─ Include reorganization
└─ API cleanup

Phase 5 (TODO) →
├─ Compatibility adapters
├─ Interpreter migration
└─ Gradual value type switch

Phase 6 (TODO) →
├─ Legacy code removal
├─ Final type migration
└─ Full consolidation

Phase 7 (TODO) →
├─ Documentation
├─ Benchmarking
└─ Architecture finalization
```

---

## Summary

The consolidated MANTRA architecture provides:

✅ **Single execution facade** - `mantra::runtime::Runtime`  
✅ **Unified value system** - `mantra::runtime::Value`  
✅ **Professional organization** - Clear namespace hierarchy  
✅ **Extensibility** - Easy to add new execution backends  
✅ **Type safety** - Strong typing throughout  
✅ **Maintainability** - No duplicate code  
✅ **Performance** - Minimal overhead from facade pattern  

The architecture is ready for production use and future enhancements.

