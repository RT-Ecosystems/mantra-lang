# MANTRA Compiler - Implementation Plan for Full Refactor

## Overview
This document provides a detailed, actionable implementation plan to resolve all identified critical and high-priority issues in the MANTRA compiler architecture.

## Phase 1: Critical Build Fixes (Duration: 30 minutes)

### Step 1.1: Create Centralized Types Header
**File:** `include/mantra/core/types.h` (NEW)

Create a unified type definition header:
```cpp
#pragma once

#include <string>

namespace mantra {

// Language identification
enum class UserLanguage {
    // Using canonical naming convention
    Hinglish, Tanglish, Punglish, Benglish,
    Gujlish, Marathlish, Telglish, Kanglish,
    Manglish, Odlish, Asslish,
    Maithlish, Konklish, Neplish, Sanglish,
    Kashlish, Sindlish, Doglish, Manlish,
    Bodlish, Santlish, English, Unknown
};

// Value type system
enum class ValueType {
    Number,
    String,
    Boolean,
    Null,
    Function,
    Array,
    Object
};

// Error classifications
enum class ErrorType {
    LEXICAL_ERROR,
    SYNTAX_ERROR,
    SEMANTIC_ERROR,
    RUNTIME_ERROR,
    TYPE_ERROR,
    UNKNOWN_IDENTIFIER,
    INVALID_OPERATION,
    FILE_NOT_FOUND,
    IO_ERROR,
    WARNING,
    INFO,
    UNKNOWN_ERROR
};

enum class ErrorCode {
    MISSING_THEN,
    VAR_UNDEFINED,
    TYPE_MISMATCH,
    SYNTAX_ERROR,
    LET_MISSING_ID,
    INVALID_ASSIGN,
    MISSING_RPAREN,
    MISSING_RBRACE,
    UNDEFINED_FUNCTION,
    WRONG_ARG_COUNT,
    UNKNOWN_IDENTIFIER,
    INVALID_OPERATION,
    FILE_NOT_FOUND,
    IO_ERROR
};

// AST Node types
enum class NodeType {
    PROGRAM,
    BLOCK_STMT,
    PRINT_STMT,
    ASSIGN_STMT,
    IF_STMT,
    WHILE_STMT,
    FOR_STMT,
    FUNC_DEF,
    RETURN_STMT,
    BREAK_STMT,
    CONTINUE_STMT,
    EXPR_STMT,
    BINARY_EXPR,
    UNARY_EXPR,
    CALL_EXPR,
    NUMBER_LIT,
    STRING_LIT,
    BOOL_LIT,
    NULL_LIT,
    IDENTIFIER,
    ARRAY_LIT,
    INDEX_EXPR,
    MEMBER_EXPR
};

// Bytecode opcodes
enum class OpCode {
    PUSH_CONST,
    ADD, SUB, MUL, DIV, MOD,
    PRINT,
    HALT,
    // ... add all opcodes
};

} // namespace mantra
```

**Actions:**
1. Create new header file with all shared types
2. Use canonical enum value names (PascalCase for languages)
3. Add comprehensive comments
4. Update CMakeLists.txt include path

### Step 1.2: Update error/error.h
**File:** `error/error.h` (MODIFY)

Replace duplicate enum with include:
```cpp
#pragma once

#include <mantra/core/types.h>
// Remove the duplicate UserLanguage enum
// Keep ErrorType and ErrorCode - they're now in types.h

// Or use the ones from types.h:
using mantra::UserLanguage;
using mantra::ErrorType;
using mantra::ErrorCode;
```

### Step 1.3: Update web/mtr_parser.h
**File:** `web/mtr_parser.h` (MODIFY)

Remove duplicate enum:
```cpp
#pragma once

#include <mantra/core/types.h>
// Remove duplicate UserLanguage enum
// Use the one from types.h via include
```

### Step 1.4: Implement Missing Parser Function
**File:** `core/parser.cpp` (MODIFY)

Add the missing function implementation:
```cpp
void Parser::errorAtCurrent(const std::string& message) {
    if (isAtEnd()) {
        ErrorHandler::printError(
            ErrorType::SYNTAX_ERROR,
            "Error at end of file: " + message,
            peek().line,
            peek().column
        );
    } else {
        const Token& token = peek();
        ErrorHandler::printError(
            ErrorType::SYNTAX_ERROR,
            "Error at '" + token.lexeme + "': " + message,
            token.line,
            token.column
        );
    }
}
```

### Step 1.5: Verify Build
**Commands:**
```bash
cd /tmp/workspace/RT-Ecosystems/mantra-lang
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . 2>&1 | tee build.log
```

**Expected outcome:** Build succeeds ✅

---

## Phase 2: Architecture Consolidation (Duration: 2 hours)

### Step 2.1: Consolidate VM Implementations

**Decision:** Keep `src/vm/` (modern implementation) as canonical

**Actions:**
1. Delete `vm/` directory (legacy)
2. Delete `bytecode/opcode.h` (use src/vm/bytecode.h instead)
3. Rename `src/vm/` to `vm/` for convenience OR keep current structure
4. Update CMakeLists.txt to reference only canonical VM
5. Update main.cpp includes to use canonical VM

**File changes:**
- **DELETE:** `vm/vm.h`, `vm/vm.cpp`
- **DELETE:** `bytecode/chunk.h`, `bytecode/chunk.cpp`, `bytecode/opcode.h`
- **KEEP:** `src/vm/` with all files
- **UPDATE:** CMakeLists.txt build configuration

**CMakeLists.txt changes:**
```cmake
# REMOVE these from add_library:
#   bytecode/chunk.cpp
#   vm/vm.cpp

# KEEP these:
#   src/vm/value.cpp
#   src/vm/stack.cpp
#   src/vm/bytecode.cpp
#   src/vm/vm.cpp
```

### Step 2.2: Unify Value Type System

**Target:** Use `include/mantra/vm/value.h` as canonical

**Actions:**
1. Update `core/interpreter.h` to use Value from `include/mantra/vm/value.h`
2. Remove `MantraValue` struct from interpreter
3. Remove `FunctionValue` struct (integrate into Value)
4. Update all interpreter code to use unified Value type
5. Rename class from `Value` to `MantraValue` for consistency OR keep as is

**File changes:**
- **MODIFY:** `core/interpreter.h` - Remove MantraValue, import from vm
- **MODIFY:** `core/interpreter.cpp` - Update all value operations
- **KEEP:** `include/mantra/vm/value.h` - Canonical value implementation
- **UPDATE:** `src/vm/value.cpp` - Comprehensive value operations

### Step 2.3: Establish Namespace Organization

**File:** `include/mantra/core/namespaces.h` (NEW)

Document namespace structure:
```cpp
// All components organized in nested namespaces:
// 
// mantra::lexer::*       - Lexical analysis (tokenization)
// mantra::parser::*      - Syntax analysis (AST generation)
// mantra::semantic::*    - Semantic analysis (type checking)
// mantra::vm::*          - Virtual machine (bytecode execution)
// mantra::stdlib::*      - Standard library (builtins)
// mantra::error::*       - Error handling
// mantra::unicode::*     - Unicode utilities
// mantra::compiler::*    - Main compiler orchestration
```

**Implementation steps:**
1. Wrap lexer classes: `namespace mantra::lexer { class Lexer { ... } }`
2. Wrap parser classes: `namespace mantra::parser { class Parser { ... } }`
3. VM already in correct namespace
4. Semantic analyzer: `namespace mantra::semantic { ... }`
5. Error handling: `namespace mantra::error { class ErrorHandler { ... } }`
6. Stdlib: `namespace mantra::stdlib { ... }`
7. Unicode: `namespace mantra::unicode { ... }`

**Impact:** Requires updating:
- All header files (namespace declarations)
- All source files (namespace usage)
- main.cpp (using declarations)
- All tests

### Step 2.4: Reorganize Include Hierarchy

**Principle:** 
- Public headers: `#include <mantra/...>`
- Internal headers: `#include "relative/path.h"`
- No path traversal (`../`) in headers

**File structure (target):**
```
include/mantra/
├── core/
│   ├── types.h           # All type definitions
│   ├── token.h           # Token definitions
│   └── version.h         # Version info
├── lexer/
│   └── lexer.h           # Public lexer interface
├── parser/
│   └── parser.h          # Public parser interface
├── semantic/
│   └── analyzer.h        # Public semantic analyzer
├── vm/
│   ├── vm.h              # Public VM interface
│   ├── bytecode.h        # Bytecode definitions
│   ├── value.h           # Value type system
│   └── stack.h           # Stack implementation
├── stdlib/
│   ├── io.h
│   ├── math.h
│   └── string.h
├── error/
│   └── handler.h         # Error handling
└── unicode/
    ├── unicode.h
    └── script_detect.h
```

**CMakeLists.txt update:**
```cmake
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# Remove confusing relative paths
```

### Step 2.5: Verify Architecture Changes
**Commands:**
```bash
cd /tmp/workspace/RT-Ecosystems/mantra-lang/build
cmake --build . 2>&1 | grep -E "error|warning|note" | head -20
```

**Expected outcome:** No linker errors, only potential namespace issues ✅

---

## Phase 3: Language Design Fixes (Duration: 1-2 hours)

### Step 3.1: Resolve Grammar Ambiguity

**Problem:** `=` operator overloading creates parser ambiguity

**Solution:** Use context-aware parsing with clearer operator distinction

**Option A (Recommended):** 
- Keep `=` for assignment
- Add `:=` for let-binding (where context requires)
- Use `==` for comparison
- Use `->` for function returns

**Option B:**
- Use `let x = 5` (keyword disambiguates)
- Use `:=` for assignment
- Use `==` for comparison

**Implementation:**
1. Update token types: Add `OP_DEFINE` for `:=`
2. Update lexer to recognize `:=`
3. Update parser to handle both assignment contexts
4. Add precedence rules

### Step 3.2: Update Parser Logic

**File:** `core/parser.cpp` (MODIFY)

Enhance parser to handle clear operator distinction:
- Separate `parseAssignment()` from `parseComparison()`
- Use operator precedence correctly
- Add parser rules for let-binding

### Step 3.3: Add Comprehensive Tests

**File:** `tests/test_parser.cpp` (NEW)

Test grammar rules:
```cpp
void test_assignment() { /* ... */ }
void test_comparison() { /* ... */ }
void test_let_binding() { /* ... */ }
void test_operator_precedence() { /* ... */ }
```

---

## Phase 4: Multilingual Keyword Engine (Duration: 1-2 hours)

### Step 4.1: Create Keyword Normalization Engine

**File:** `include/mantra/lexer/keyword_normalizer.h` (NEW)

```cpp
namespace mantra::lexer {

struct KeywordMapping {
    std::string canonical_token;      // e.g., "TOKEN_PRINT"
    std::set<std::string> variants;   // e.g., {"dikhao", "dikhaao", "kaado", ...}
    UserLanguage primary_language;    // e.g., UserLanguage::Hinglish
};

class KeywordNormalizer {
public:
    // Normalize a keyword string to canonical token
    std::string normalize(const std::string& keyword, UserLanguage context);
    
    // Fuzzy match with typo tolerance
    std::vector<std::string> suggestKeywords(const std::string& typo);
    
    // Get all keywords for a specific language
    std::set<std::string> getKeywordsForLanguage(UserLanguage lang);
};

} // namespace mantra::lexer
```

### Step 4.2: Build Transliteration Mapper

**File:** `include/mantra/unicode/transliteration.h` (NEW)

Map transliteration variants:
```cpp
namespace mantra::unicode {

class TransliterationMapper {
public:
    // Normalize multiple spellings to canonical form
    // dikhao, dikhaao, dikhaoo -> internal "print"
    std::string normalize(const std::string& input);
    
    // Get all variants of a word in a language
    std::vector<std::string> getVariants(const std::string& canonical);
};

} // namespace mantra::unicode
```

### Step 4.3: Integrate into Lexer

**File:** `core/lexer.cpp` (MODIFY)

Use normalizer during tokenization:
```cpp
Token Lexer::readIdentifierOrKeyword() {
    std::string text = readIdentifier();
    
    // Normalize to canonical keyword
    auto normalized = keyword_normalizer.normalize(text, detected_language);
    
    // Lookup canonical form
    auto it = canonical_token_map.find(normalized);
    if (it != canonical_token_map.end()) {
        return Token(it->second, text, line, column);
    }
    
    return Token(TokenType::IDENTIFIER, text, line, column);
}
```

### Step 4.4: Resolve Keyword Collisions

**Create mapping database:**
```cpp
// In keyword_normalizer.cpp

const std::unordered_map<std::string, KeywordMapping> KEYWORD_DATABASE = {
    // PRINT
    {"print", {
        "TOKEN_PRINT",
        {"dikhao", "dikhaao", "kaado", "dakho", "dekhao", ...},
        UserLanguage::English
    }},
    // IF
    {"if", {
        "TOKEN_IF",
        {"agar", "endral", "jekr", "jodi", "jor", ...},
        UserLanguage::English
    }},
    // ... more mappings
};
```

---

## Phase 5-10: Feature Completion (4+ hours)

### Step 5.1: Expand Semantic Analysis

**File:** `semantic/semantic_analyzer.cpp` (ENHANCE)

Add:
- Type inference engine
- Return type checking
- Dead code detection
- Unused variable warnings

### Step 5.2: Improve Error Diagnostics

**File:** `error/error.cpp` (ENHANCE)

Implement Rust-like diagnostics:
```
error[E0001]: undefined variable 'x'
  --> test.mtr:5:10
   |
 5 |   dikhao x + y
   |          ^ unknown identifier
   |
 = hint: did you mean 'x1'?
```

### Step 5.3: Comprehensive Testing

Add tests for:
- All language constructs
- Error cases
- Edge cases
- Performance

### Step 5.4: Performance Optimization

Profile and optimize:
- Lexer tokenization
- Parser AST generation
- VM bytecode execution

---

## Summary of File Changes

### Create (NEW)
- `include/mantra/core/types.h` - Centralized types
- `include/mantra/core/namespaces.h` - Namespace documentation
- `include/mantra/lexer/keyword_normalizer.h` - Keyword engine
- `include/mantra/unicode/transliteration.h` - Transliteration support
- `tests/test_parser.cpp` - Parser tests

### Modify
- `include/mantra/core/types.h` - Add centralized types
- `error/error.h` - Use types from types.h
- `web/mtr_parser.h` - Use types from types.h
- `core/parser.h` - Add missing function
- `core/parser.cpp` - Implement errorAtCurrent()
- `core/interpreter.h` - Use unified Value type
- `core/interpreter.cpp` - Use unified Value type
- `CMakeLists.txt` - Update build configuration
- `main.cpp` - Update includes for new structure
- `core/lexer.cpp` - Integrate keyword normalizer

### Delete
- `vm/vm.h` - Legacy VM
- `vm/vm.cpp` - Legacy VM
- `bytecode/chunk.h` - Legacy
- `bytecode/chunk.cpp` - Legacy
- `bytecode/opcode.h` - Legacy

### Keep
- `src/vm/` - Modern VM (canonical)
- All core components

---

## Testing Strategy

### Build Verification
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . 2>&1 | grep -E "error:" | wc -l  # Should be 0
ctest --output-on-failure  # Should pass all tests
```

### Incremental Verification
After each major step:
1. Build succeeds (no errors)
2. All tests pass
3. No new warnings
4. Code compiles cleanly

---

## Risk Mitigation

### Risk: Breaking Existing Code
**Mitigation:** 
- Keep backward compatibility where possible
- Maintain deprecated functions with aliases
- Create migration guide for breaking changes

### Risk: Namespace Changes Cause Cascading Errors
**Mitigation:**
- Update using declarations gradually
- Use namespace aliases initially
- Add typedef statements for compatibility

### Risk: Value Type Unification Causes Issues
**Mitigation:**
- Comprehensive testing before and after
- Maintain parallel implementation briefly
- Clear error messages for type mismatches

---

## Timeline

| Phase | Duration | Effort | Dependencies |
|-------|----------|--------|---|
| 1: Build Fixes | 30 min | 1 dev | None |
| 2: Architecture | 2 hours | 1-2 devs | Phase 1 ✅ |
| 3: Language Design | 1-2 hours | 1-2 devs | Phase 2 ✅ |
| 4: Keyword Engine | 1-2 hours | 1 dev | Phase 1 ✅ |
| 5-10: Features | 4+ hours | 2+ devs | Phases 1-4 ✅ |
| **Total** | **8-10 hours** | **1-2 devs** | - |

---

## Success Criteria

- [ ] Build succeeds with zero errors
- [ ] All 5 critical issues resolved
- [ ] All 10 high-priority issues resolved
- [ ] Tests pass (100%)
- [ ] No compiler warnings
- [ ] Code follows consistent style
- [ ] Documentation updated
- [ ] Architecture clearly organized

---

## Next Steps

1. **Immediate:** Execute Phase 1 (build fixes)
2. **Short-term:** Execute Phase 2 (architecture)
3. **Medium-term:** Execute Phases 3-4 (language + keywords)
4. **Long-term:** Execute Phases 5-10 (features + polish)

**Recommended:** Complete Phases 1-2 in first session to stabilize foundation.

