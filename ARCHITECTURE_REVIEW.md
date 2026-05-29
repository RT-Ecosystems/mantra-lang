# MANTRA Architecture Review - Detailed Analysis

**Date:** May 29, 2026  
**Reviewer Role:** Senior Compiler Architect  
**Assessment Level:** Deep dive into design decisions and their implications

---

## 1. Current Architecture Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                        MANTRA Compiler Stack                        │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  INPUT: .mtr files (Unicode text in any Indian language)           │
│    ↓                                                               │
│  ┌──────────────────────────────────────────────────────────────┐  │
│  │ LEXER (648 LOC) - Tokenization                              │  │
│  │ • UTF-8 aware                                               │  │
│  │ • Keyword normalization (250+ keywords)                     │  │
│  │ • 22 languages supported                                    │  │
│  │ Status: ✅ PRODUCTION QUALITY                              │  │
│  └────────────────┬─────────────────────────────────────────────┘  │
│                   ↓                                                 │
│  ┌──────────────────────────────────────────────────────────────┐  │
│  │ PARSER (683 LOC) - Syntax Analysis                          │  │
│  │ • Recursive descent                                          │  │
│  │ • Precedence climbing for expressions                        │  │
│  │ • Error recovery                                             │  │
│  │ Status: ✅ PRODUCTION QUALITY                              │  │
│  └────────────────┬─────────────────────────────────────────────┘  │
│                   ↓                                                 │
│  ┌──────────────────────────────────────────────────────────────┐  │
│  │ AST (561 LOC header) - Intermediate Representation          │  │
│  │ • 21 node types                                              │  │
│  │ • Virtual visitor pattern                                    │  │
│  │ • Location tracking                                          │  │
│  │ Status: ✅ SOLID                                            │  │
│  └────────────────┬─────────────────────────────────────────────┘  │
│                   ↓                                                 │
│  ┌──────────────────────────────────────────────────────────────┐  │
│  │ SEMANTIC ANALYZER (1,740 LOC) - Type/Scope Analysis        │  │
│  │ • Symbol table management                                    │  │
│  │ • Scope tracking                                             │  │
│  │ • Type checking (INCOMPLETE)                                │  │
│  │ Status: ⚠️ PARTIAL IMPLEMENTATION                          │  │
│  └────────────────┬─────────────────────────────────────────────┘  │
│                   ↓                                                 │
│  ┌──────────────────────────────────────────────────────────────┐  │
│  │ EXECUTION LAYER (Multiple Backends)                         │  │
│  ├────────────────────────────────────────────────────────────┤  │
│  │ Option A: INTERPRETER (992 LOC)                            │  │
│  │ • Tree-walking AST execution                                │  │
│  │ • Environment-based scoping                                 │  │
│  │ • Exception-based control flow                              │  │
│  │ Status: ✅ WORKING (currently used)                        │  │
│  │                                                              │  │
│  │ Option B: BYTECODE COMPILER + VM (STUB)                    │  │
│  │ • Bytecode compiler: NOT IMPLEMENTED                        │  │
│  │ • VM execution: NOT IMPLEMENTED                             │  │
│  │ Status: ❌ INCOMPLETE                                      │  │
│  └────────────────┬─────────────────────────────────────────────┘  │
│                   ↓                                                 │
│  ┌──────────────────────────────────────────────────────────────┐  │
│  │ STANDARD LIBRARY (~1,143 LOC)                              │  │
│  │ • Math functions                                             │  │
│  │ • I/O operations                                             │  │
│  │ • String utilities                                           │  │
│  │ Status: ⚠️ BASIC                                           │  │
│  └────────────────┬─────────────────────────────────────────────┘  │
│                   ↓                                                 │
│  ┌──────────────────────────────────────────────────────────────┐  │
│  │ OUTPUT: Program results or runtime errors                   │  │
│  └──────────────────────────────────────────────────────────────┘  │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 2. Component Architecture Deep Dive

### 2.1 Lexer Architecture

**Design Pattern:** Hand-written scanner with lookahead

**Strengths:**
- Single-pass design with O(n) complexity
- Character-by-character processing with lookahead
- Efficient keyword mapping via hash table
- Proper handling of multi-byte UTF-8 characters
- Cross-platform line ending support

**Weaknesses:**
- No lexer modes/contexts (would be useful for nested strings)
- Limited error recovery (just reports and continues)
- No token buffering (direct token stream)

**Recommendations:**
- Add lexer context stack for nested constructs
- Implement better error recovery with suggestions
- Consider adding pragma/directive support

---

### 2.2 Parser Architecture

**Design Pattern:** Recursive descent with precedence climbing

**Strengths:**
- Clean, readable recursive descent structure
- Proper operator precedence via precedence climbing
- Good error messages with location tracking
- Handles complex expressions correctly
- Statement parsing well-structured

**Critical Issue: Grammar Ambiguity**

The parser has a **fundamental design flaw** with the `=` operator:

```
agar a = 10 tab          // ASSIGNMENT (interpreted as: if (a = 10) then)
  // vs
agar a == 10 tab         // COMPARISON (correct conditional)
```

The grammar ambiguously allows assignment in expression context, which:
- Creates two syntactically valid interpretations
- Leads to confusing semantics
- Makes static analysis difficult
- Creates error-prone code patterns

**Resolution Options:**
1. **Option A:** Disallow assignment in conditional expressions (simplest)
2. **Option B:** Require parentheses: `agar (a = 10) tab` (explicit)
3. **Option C:** Separate assignment from expression-statement (best)

**Weaknesses:**
- Single token lookahead (limits parsing strategies)
- Limited error recovery (basic synchronization)
- No precedence table (hardcoded in functions)
- No operator table for extension

**Recommendations:**
1. **Fix grammar ambiguity** immediately
2. Implement precedence table for easier modification
3. Add error recovery with context stacks
4. Support lookahead > 1 for future extensions

---

### 2.3 AST Architecture

**Design Pattern:** Visitor pattern with virtual methods

**Structure:**
```cpp
struct MantraNode {
    NodeType type;
    int line, column;
    virtual std::string toStringIndented(int indent) const = 0;
};

// Specific node types derive from MantraNode
```

**Strengths:**
- Clear node type hierarchy
- Location tracking built-in
- Virtual visitor interface
- Debug toString() methods

**Weaknesses:**
- No type annotations in AST
- No attributes/metadata support
- No span information (only start position)
- No annotation points for optimization
- Missing abstract base nodes

**Architectural Flaw:**
The AST stores `NodeType` enum AND virtual dispatch. This is redundant and error-prone:
```cpp
// Two ways to identify node type:
node->type == NodeType::BINARY_EXPR    // Enum check
dynamic_cast<BinaryExprNode*>(node)    // Virtual dispatch
```

**Recommendations:**
1. Use ONLY virtual dispatch (cleaner design)
2. Add span information (start + end position)
3. Add symbol table attachment points
4. Add type annotation fields
5. Support attribute annotations

---

### 2.4 Semantic Analyzer Architecture

**Design Pattern:** Multiple-pass analysis with symbol tables

**Current Implementation:**
- Pass 1: Collect function declarations
- Pass 2: Analyze statements
- Pass 3: Type checking (STUB - not implemented)

**Strengths:**
- Symbol table per scope
- Distinction between variables, functions, builtins
- Scope stack for lexical scoping
- Function signature tracking

**Critical Issues:**

1. **Type Checking Not Implemented**
   ```cpp
   bool SemanticAnalyzer::typeCheck(const MantraNode& node) {
       // Empty implementation!
   }
   ```
   The entire type checking pass is stubbed out.

2. **Type Inference Not Implemented**
   ```cpp
   std::shared_ptr<Type> TypeChecker::inferType(const MantraNode& node) {
       // Unused parameter 'node' - stub implementation
   }
   ```

3. **Incomplete Scope Management**
   - Only tracks variables in current scope
   - Doesn't validate across function boundaries
   - No cross-function optimization hints

**Weaknesses:**
- Single-pass for most analysis (lacks multi-pass opportunities)
- Limited error recovery
- No error accumulation (stops on first error)
- No type constraint propagation
- No data flow analysis

**Recommendations:**
1. **Implement type checking enforcement**
2. **Implement type inference engine**
3. Add multi-pass analysis pipeline
4. Implement error accumulation
5. Add data flow analysis for optimization

---

### 2.5 Type System Architecture

**Three Parallel Type Systems Found:**

**System 1: `MantraValue` (core/interpreter.h)**
```cpp
enum class ValueType { Number, String, Boolean, Null, Function, Array, Object };
struct MantraValue {
    ValueType type;
    double number_value;
    std::string string_value;
    bool bool_value;
    std::shared_ptr<FunctionValue> function;
    std::vector<MantraValue> array_value;
    // NOTE: object_value removed temporarily
};
```
- **Purpose:** Runtime value representation in interpreter
- **Status:** ✅ Working
- **Problem:** Missing object/dict support

**System 2: `Value` (include/mantra/vm/value.h)**
```cpp
enum class ValueType { Number, String, Boolean, Null, Function, Array, Object };
class Value {
    // Modern variant-based implementation
};
```
- **Purpose:** Intended for VM execution
- **Status:** ✅ Compiles, but not used
- **Problem:** Duplicate of System 1

**System 3: `Value` (include/mantra/runtime/types.h)**
```cpp
class Value {
    // Runtime facade layer
};
```
- **Purpose:** Unified interface for runtime
- **Status:** ⚠️ Incomplete
- **Problem:** Not integrated with either System 1 or 2

**Critical Architectural Flaw:**
These three systems have:
- Identical type enums
- Similar structure but different implementation details
- No clear relationship or unification strategy
- Different semantics (reference vs value semantics)
- No interoperability layer

**Cost of This Duplication:**
- **Maintenance:** Bugs in one must be fixed in all three
- **Integration:** VM can't use interpreter values
- **Testing:** Must test all three paths
- **Scalability:** Adding new value type requires three changes

**Recommendation:**
Consolidate into **single unified system:**
```cpp
// include/mantra/runtime/value.h
namespace mantra::runtime {
    enum class ValueType { /* shared */ };
    class Value { /* single implementation */ };
}
```

---

### 2.6 Execution Model Architecture

#### 2.6.1 Interpreter (Tree-Walking)

**Design Pattern:** Visitor pattern over AST

**Strengths:**
- Simple, understandable execution model
- Easy to debug (direct AST traversal)
- Good error messages with location tracking
- Proper lexical scoping via environment chain
- Exception-based control flow (elegant)

**Implementation Details:**
```
Environment Chain:
┌────────────────┐
│ Local Scope    │ ← Current
├────────────────┤
│ Enclosing      │
├────────────────┤
│ Global Scope   │ ← Builtins + User globals
└────────────────┘

Call Stack:
depth = 0    // Entry point
  ↓
depth = 1    // Function call
  ↓
depth = 2    // Recursive call
  ...
depth < 1024 // Maximum depth
```

**Weaknesses:**
- No optimization passes
- No tail call optimization
- No caching of computed values
- Repeated work for same expressions
- No dead code elimination

**Performance Characteristics:**
- **Startup:** Immediate (no compilation)
- **Memory:** Full AST in memory
- **Speed:** ~100-1000x slower than native code
- **Scalability:** Linear with AST size

---

#### 2.6.2 VM/Bytecode Architecture (STUB)

**Design Pattern:** Stack-based virtual machine

**Intended Architecture:**
```
┌────────────────────────────────────────────┐
│ Bytecode Compiler (NOT IMPLEMENTED)        │
│ • AST → Bytecode                           │
│ • Instruction selection                    │
│ • Register allocation                      │
└────────────────┬───────────────────────────┘
                 ↓
┌────────────────────────────────────────────┐
│ Bytecode Format                            │
│ • Instruction stream                       │
│ • Constant pool                            │
│ • Function table                           │
│ • Debug information                        │
└────────────────┬───────────────────────────┘
                 ↓
┌────────────────────────────────────────────┐
│ VM Execution (NOT IMPLEMENTED)             │
│ • Instruction dispatch loop                │
│ • Stack manipulation                       │
│ • Function call handling                   │
│ • Exception handling                       │
└────────────────────────────────────────────┘
```

**Current Status:**
- ✅ Value types defined
- ✅ Stack representation designed
- ✅ Bytecode format sketched
- ✅ VM loop structure planned
- ❌ Compiler not implemented
- ❌ Bytecode generation not implemented
- ❌ VM execution returns error

**Issues with Current Design:**
1. No bytecode compiler implementation
2. No instruction selection strategy
3. No register allocation scheme
4. No optimization pipeline
5. No debug information format

**Why VM Wasn't Completed:**
- Complex compared to interpreter
- Requires intermediate representation design
- Needs instruction set definition
- Requires testing infrastructure
- Would need performance benchmarking

---

### 2.7 Standard Library Architecture

**Current Structure:**
```
stdlib/
├── math.cpp    (317 LOC, 11 functions)
├── io.cpp      (427 LOC, 8 functions)
├── string.cpp  (197 LOC, 6 functions)
└── network.cpp (100 LOC, STUBS ONLY)
```

**Math Library Functions:**
- Constants: PI, E
- Trigonometric: sin, cos, tan
- Exponential: exp, log, sqrt
- Rounding: floor, ceil, round
- Utility: max, min, abs

**I/O Library Functions:**
- print (variadic)
- input (read line)
- open, read, write, close (FILE STUBS - NOT WORKING)
- format (string formatting)

**String Library Functions:**
- length, concat
- substring, split
- toUpperCase, toLowerCase

**Network Library Functions:**
- COMPLETELY STUBBED (no implementation)

**Weaknesses:**
1. No error handling in library functions
2. File I/O not implemented
3. Network functions not implemented
4. No type conversions
5. Limited string operations
6. No collection operations

**Missing Standard Features:**
- No JSON library
- No regex support
- No date/time
- No compression
- No cryptography
- No database access
- No HTTP client
- No threading

---

### 2.8 Error Handling Architecture

**Design Pattern:** Exception-based error handling

**Strengths:**
- Control flow exceptions (return, break, continue)
- Error messages with location tracking
- Bilingual error output (Hindi/English)
- ANSI colored output
- Clear error context

**Weaknesses:**
- No error recovery in semantic analysis
- No error accumulation (stops at first error)
- No error codes for programmatic handling
- No structured error types
- No stack traces for debugging

**Error Categories:**
1. **Lexical Errors** - Tokenization failures
2. **Syntactic Errors** - Parse failures
3. **Semantic Errors** - Type/scope violations
4. **Runtime Errors** - Execution failures

---

### 2.9 Unicode Architecture

**Design Pattern:** UTF-8 encoding with codepoint conversion

**Components:**
1. **Unicode Handler** - UTF-8 encode/decode
2. **Script Detector** - Identify Indian scripts
3. **Character Classification** - Digit/letter detection

**Strengths:**
- Full UTF-8 support
- Multi-byte character handling
- 9 Indian scripts supported
- Proper error handling for invalid UTF-8
- Cross-platform compatibility

**Weaknesses:**
- Limited script detection confidence
- No normalization (NFC/NFD)
- No combining character support
- No bidirectional text handling
- No grapheme cluster awareness

**Recommendation:**
Consider using ICU (International Components for Unicode) library for production use.

---

## 3. Architectural Patterns and Anti-Patterns

### Good Patterns
✅ **Visitor Pattern** - AST traversal
✅ **Factory Methods** - Value creation
✅ **Environment Chain** - Scope management
✅ **Token Enumerations** - Syntax representation
✅ **Location Tracking** - Error reporting

### Anti-Patterns Found
❌ **Multiple Inheritance** - Value types duplicated
❌ **God Objects** - Interpreter handles too much
❌ **Dead Code** - Legacy VM not removed
❌ **Type Duplication** - Three value systems
❌ **Mixed Concerns** - Parser does error handling AND syntax analysis

### Design Issues
⚠️ **Tight Coupling** - Parser couples to specific keywords
⚠️ **Hard to Extend** - Adding new operators requires parser changes
⚠️ **Inconsistent APIs** - Different namespace patterns
⚠️ **No Plugin Architecture** - Can't add external functions easily

---

## 4. Extensibility Analysis

### Current Extensibility

**Adding a new built-in function:**
1. Add to stdlib/XXX.cpp
2. Register in interpreter.cpp
3. Add tests
4. **Effort: Low (15 minutes)**

**Adding a new language:**
1. Add keywords to KeywordNormalizer
2. Add token class mappings
3. Add test cases
4. **Effort: Medium (30 minutes)**

**Adding a new operator:**
1. Add token type
2. Add parser precedence function
3. Add evaluator case
4. Add semantic checks
5. **Effort: Medium (1 hour)**

**Adding a new data type:**
1. Add ValueType enum
2. Update all three value systems 😱
3. Add operators/methods
4. Add semantics
5. **Effort: High (4 hours) - due to duplication**

**Adding a new statement type:**
1. Add AST node
2. Add parser function
3. Add semantic analysis
4. Add interpreter case
5. **Effort: Medium (1 hour)**

### Extensibility Grade: ⚠️ **FAIR**
- Low friction for small changes
- High friction for structural changes
- Duplication makes some changes expensive

---

## 5. Scalability Analysis

### Can MANTRA Scale?

**For Small Programs (100 LOC):**
- ✅ No problems
- Execution: sub-second
- Memory: negligible

**For Medium Programs (1000 LOC):**
- ✅ Should work
- Execution: few seconds
- Memory: reasonable

**For Large Programs (10,000+ LOC):**
- ⚠️ Potential issues
- AST memory may grow large
- No optimization = slower execution
- Stack limit (1024) might be hit
- No module system

**Architectural Limits:**
- Call stack depth: 1024 frames (reasonable)
- Variable count: Unlimited (env chain)
- Function count: Unlimited
- Array size: Limited by available memory
- String size: Limited by available memory
- **No inherent scalability issues**, but missing optimizations

---

## 6. Maintainability Analysis

### Code Organization
- **Core paths:** Clear and logical
- **Frontend:** Well-structured
- **Backend:** Inconsistent (multiple paths, duplication)
- **Overall:** Moderate maintainability

### Code Comprehension
- **Well-commented:** 0 TODOs (good - or bad? no guidance on future work)
- **Naming:** Consistent and clear
- **Structure:** Logical function ordering
- **Documentation:** Limited API docs

### Change Impact Analysis
- **Low Risk:** Adding keywords (isolated change)
- **Medium Risk:** Adding functions (must register in 2 places)
- **High Risk:** Changing value type (affects entire codebase)
- **High Risk:** Changing parser precedence (can break existing code)

### Technical Debt
- Three value systems: **HIGH DEBT**
- Incomplete type checking: **HIGH DEBT**
- Legacy VM code: **LOW DEBT**
- Limited tests: **MEDIUM DEBT**

---

## 7. Performance Bottlenecks

### Identified Bottlenecks

1. **AST Traversal** (Interpreter)
   - Every statement requires tree walk
   - No caching of expressions
   - Repeated evaluations of constants
   - **Impact:** 10-100x slowdown

2. **Environment Lookups**
   - Linear search through scope chain
   - No hash table optimization
   - **Impact:** 2-5x slowdown for variable access

3. **Function Calls**
   - Environment creation per call
   - Parameter copying
   - No inlining
   - **Impact:** 5-10x overhead per call

4. **No Optimization Passes**
   - Constant folding not done
   - Dead code not eliminated
   - Loop invariants not hoisted
   - **Impact:** Variable, program-dependent

### Scalability Recommendations

For **interpreter-based execution**:
- Add constant folding in semantic analysis
- Add dead code elimination
- Cache global/function lookups
- Implement symbol table optimization

For **VM-based execution** (if implemented):
- Add bytecode compiler optimizations
- Implement instruction scheduling
- Add peephole optimization
- Implement JIT for hot paths

---

## 8. Language Direction Analysis

### Current Direction Indicators

**What the architecture suggests:**

1. **Interpreter-First Design** - Easy to debug, but slow
2. **No JIT Plans** - Current code doesn't assume JIT
3. **Dynamic Typing** - All values runtime-checked
4. **No Modules** - Flat namespace (everything global or local)
5. **No OOP** - No classes, inheritance, or polymorphism
6. **Functional Elements** - First-class functions, lexical scoping

### Three Possible Futures

**Path A: Simple Scripting Language**
- Keep interpreter
- Add modules and imports
- Expand standard library
- Add package manager
- **Effort:** Medium
- **Timeline:** 2-3 months

**Path B: Bytecode VM Language**
- Finish VM implementation
- Add bytecode compiler
- Add basic optimization
- Performance focus
- **Effort:** High
- **Timeline:** 3-4 months

**Path C: Compiled Language (LLVM Backend)**
- Implement LLVM codegen
- Add static typing option
- Add optimization passes
- Performance/safety focus
- **Effort:** Very High
- **Timeline:** 4-6 months

### Recommendation
**Path A (Simple Scripting)** is most aligned with current codebase. Path C would require major architecture redesign.

---

## 9. Dependency Analysis

### External Dependencies
- ❌ **ZERO external dependencies** (pure C++17)
- ✅ Only CMake for building
- ✅ Self-contained

### Internal Dependencies

**Circular Dependency Risk: LOW**
- Lexer → Parser: ✅ Clean
- Parser → Semantic: ✅ Clean
- Semantic → Interpreter: ✅ Clean
- Interpreter → Stdlib: ✅ Clean
- No circular dependencies detected

### Coupling Analysis

**Parser-to-Keywords:** ✅ Loose
- Via keyword normalizer
- Easy to add new keywords

**Interpreter-to-Stdlib:** ✅ Loose
- Via registration pattern
- Can add new functions

**Semantic-to-Interpreter:** ❌ Tight
- Both use AST directly
- Limited by AST structure

**Value Types:** ❌ Very Tight
- Three copies of similar code
- Any change needed in multiple places

---

## 10. Testing Architecture

### Current Test Structure
```
tests/
├── test_lexer.cpp              (239 LOC, ~40% coverage)
├── test_keyword_normalizer.cpp (157 LOC, ~80% coverage)
├── test_typesystem.cpp         (308 LOC, ~30% coverage)
├── test_vm.cpp                 (96 LOC, ~20% coverage)
├── test_stdlib_math.cpp        (94 LOC, ~50% coverage)
└── test_semantic.cpp           (75 LOC, ~25% coverage)
```

**Test Approach:**
- Unit tests for each component
- No integration tests
- No end-to-end tests
- No performance benchmarks

**Missing Test Categories:**
- ❌ Parser edge cases (1000+ cases needed)
- ❌ Semantic error cases (comprehensive)
- ❌ Integration tests (multi-component)
- ❌ End-to-end language tests
- ❌ Performance benchmarks
- ❌ Stress tests (large programs)
- ❌ Error recovery tests

**Test Quality:** ⚠️ **MINIMAL**
- Coverage insufficient
- No negative test cases
- No edge case coverage
- No error condition testing

---

## 11. Architectural Recommendations

### Priority 1: Consolidation (Critical)

**Action:** Merge three value types into one
```cpp
// New unified system
namespace mantra::runtime {
    enum class ValueType { /* shared */ };
    class Value { /* single impl */ };
    class Function { /* function impl */ };
    class Environment { /* scope management */ };
}
```
**Effort:** 4-6 hours  
**Impact:** Unblocks VM integration

---

### Priority 2: Completion (High)

**Action 1:** Implement type checking
- Enforce type compatibility
- Report type errors
- Add type annotations to AST

**Action 2:** Implement type inference
- Infer literal types
- Propagate through expressions
- Handle function returns

**Effort:** 8-10 hours  
**Impact:** Improves type safety

---

### Priority 3: Cleanup (Medium)

**Action 1:** Remove legacy VM code
- Delete `vm/` directory
- Update CMakeLists.txt
- Remove legacy_vm namespace

**Action 2:** Fix compiler warnings
- Remove unused parameters
- Add `[[maybe_unused]]` where needed
- Compile with `-Wall -Wextra -Wpedantic`

**Effort:** 1-2 hours  
**Impact:** Improves code quality

---

### Priority 4: Expansion (Medium)

**Action:** Expand standard library
- Implement file I/O (currently stubs)
- Implement network functions
- Add collection operations
- Add type conversion functions

**Effort:** 6-8 hours  
**Impact:** Improves usability

---

## 12. Conclusion

### Architecture Summary

**Strengths:**
- ✅ Clean frontend (lexer, parser)
- ✅ Solid AST design
- ✅ Working interpreter
- ✅ Good error messages
- ✅ Excellent keyword system

**Weaknesses:**
- ❌ Three value type systems (duplication)
- ❌ Type checking not implemented
- ❌ VM not implemented
- ❌ Incomplete semantic analysis
- ❌ Limited standard library

**Verdict:** The architecture is **fundamentally sound** but has significant duplication and incomplete subsystems. The foundation is solid enough to build upon with focused engineering effort.

**Recommended Next Steps:**
1. Consolidate value types (4-6 hours)
2. Implement type checking (8-10 hours)
3. Remove dead code (1-2 hours)
4. Expand standard library (6-8 hours)
5. Consider VM implementation (12+ hours, future)

**Total Effort to Stabilize:** ~40-50 hours

### Architecture Grade: **6.5/10**
- Good foundations
- Significant duplication
- Incomplete systems
- Needs consolidation

With focused effort, MANTRA's architecture can become **8.5/10** (solid, maintainable, extensible).
