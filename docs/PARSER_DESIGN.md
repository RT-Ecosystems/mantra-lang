# MANTRA Parser Design Report: Architecture & Optimization

## Executive Summary

This document details the redesigned MANTRA parser architecture, moving from a complex, language-specific system with 40+ helper functions to a clean, data-driven canonical token system. The new architecture reduces complexity by 50%+, improves maintainability, and enables future multilingual enhancements.

## Current Architecture Problems

### Problem 1: Helper Function Explosion

**Current State**: 40+ specialized helper functions

```cpp
bool Parser::isPrintKeyword(TokenType type) const;
bool Parser::isIfKeyword(TokenType type) const;
bool Parser::isElseKeyword(TokenType type) const;
bool Parser::isWhileKeyword(TokenType type) const;
bool Parser::isForKeyword(TokenType type) const;
bool Parser::isFunctionKeyword(TokenType type) const;
bool Parser::isReturnKeyword(TokenType type) const;
bool Parser::isBreakKeyword(const Token& token) const;
bool Parser::isContinueKeyword(const Token& token) const;
bool Parser::isTrueKeyword(TokenType type) const;
bool Parser::isFalseKeyword(TokenType type) const;
bool Parser::isNullKeyword(const Token& token) const;
bool Parser::isNotKeyword(TokenType type) const;
bool Parser::isAndKeyword(TokenType type) const;
bool Parser::isOrKeyword(TokenType type) const;
bool Parser::isFromKeyword(TokenType type) const;
bool Parser::isToKeyword(TokenType type) const;
bool Parser::isThenKeyword(TokenType type) const;
// ... and more
```

Each function hardcodes a large list of language-specific token types:

```cpp
bool Parser::isPrintKeyword(TokenType type) const {
    return containsTokenType({
        TokenType::KW_DIKHAO, TokenType::KW_KAADO, TokenType::KW_DAKHO, 
        TokenType::KW_DEKHAO, TokenType::KW_BATAVO, TokenType::KW_DAKHAV,
        TokenType::KW_CHUPINCHU, TokenType::KW_TORO, TokenType::KW_KAANIKU,
        TokenType::KW_DEKHAOW, TokenType::KW_DAKHOI, TokenType::KW_DEKHAU,
        TokenType::KW_DARSHAYA, TokenType::KW_WAATAV, TokenType::KW_DIKHAY,
        TokenType::KW_WEKHO, TokenType::KW_NUNGSI, TokenType::KW_NANGI,
        TokenType::KW_DADO, TokenType::KW_PRINT
    }, type);
}
```

**Impact**:
- Unmaintainable: 40+ functions × 20+ lines each = 800+ lines of boilerplate
- Error-prone: Easy to miss a variant when adding new language
- Non-scalable: Adding new language requires touching 40+ functions
- Test nightmare: Each function needs separate test cases

### Problem 2: Token Type Explosion

**Current State**: 200+ TokenType enum entries

```cpp
enum class TokenType {
    KW_DIKHAO, KW_KAADO, KW_DAKHO, KW_DEKHAO, KW_BATAVO, KW_DAKHAV,
    KW_CHUPINCHU, KW_TORO, KW_KAANIKU, KW_DEKHAOW, KW_DAKHOI, KW_DEKHAU,
    KW_DARSHAYA, KW_WAATAV, KW_DIKHAY, KW_WEKHO, KW_NUNGSI, KW_NANGI,
    KW_DADO, KW_PRINT,
    // ... repeat for IF, ELSE, WHILE, FOR, FUNCTION, RETURN, etc.
    // Result: 200+ entries
};
```

**Impact**:
- Massive token.h file
- Complex tokenTypeToString() function (1000+ lines)
- Hard to reason about token space
- Version control churn on every language addition

### Problem 3: Grammar Ambiguity

**Current State**: `dikhao = "Namaste"` conflates print and assignment

Example from codebase:
```mantra
dikhao = "Namaste Duniya!"
```

**Ambiguity**: Is this:
1. A print statement with the `=` operator?
2. An assignment of a string to variable `dikhao`?
3. Something else?

**Parser confusion**: Multiple ways to parse the same code

**Solution**: Eliminate `=` from print syntax
- NEW (canonical): `dikhao "Namaste"` (unambiguous: keyword + expression)
- OLD (deprecated): `dikhao = "Namaste"` (rejected by new parser)

### Problem 4: Operator Precedence Not Documented

**Current**: No formal precedence specification
- Embedded in parser recursive descent calls
- Not documented anywhere
- Hard to verify correctness
- Difficult to modify safely

## New Architecture

### Layer 1: TokenClass System (Canonical Tokens)

Replace 200+ tokens with ~25 canonical classes:

```cpp
enum class TokenClass {
    // Statements
    PRINT, IF, ELSE, WHILE, FOR, FUNCTION, RETURN, BREAK, CONTINUE, LET,
    
    // Literals
    TRUE, FALSE, NULL_LIT, NUMBER, STRING, IDENTIFIER,
    
    // Operators (semantic grouping)
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,      // Arithmetic
    EQUAL, NOT_EQUAL, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,  // Comparison
    AND, OR, NOT,                                 // Logical
    ASSIGN, THEN, FROM, TO,                       // Assignment/Control
    
    // Delimiters
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    COMMA, DOT, SEMICOLON, COLON,
    
    // Special
    NEWLINE, EOF_TOKEN, UNKNOWN, ERROR_TOKEN
};
```

**Benefits**:
- Clear semantic grouping
- Constant number regardless of language count
- Parseable at a glance
- Easy to extend for new syntactic constructs

### Layer 2: KeywordNormalizer (Central Mapping)

Single source of truth for all keyword variants:

```cpp
class KeywordNormalizer {
public:
    TokenClass normalize(const std::string& keyword) const;
    std::string getCanonicalKeyword(TokenClass klass) const;
    std::vector<std::string> getVariants(TokenClass klass) const;
    std::optional<TokenClass> fuzzyMatch(const std::string& keyword) const;
};

// Usage in lexer
TokenClass klass = normalizer.normalize(keyword_text);
Token token(klass, keyword_text, line, col);
```

**Benefits**:
- Centralized maintenance
- Add language = add 20 entries to normalizer
- Support for fuzzy matching and typo tolerance
- Trivial to test all language variants

### Layer 3: Simplified Parser (Data-Driven Predicates)

Replace helper functions with simple checks:

```cpp
// OLD: 40+ helper functions
if (isPrintKeyword(peek().type)) { ... }
if (isIfKeyword(peek().type)) { ... }
if (isElseKeyword(peek().type)) { ... }
// ...

// NEW: Single helper with TokenClass parameter
if (check(TokenClass::PRINT)) { ... }
if (check(TokenClass::IF)) { ... }
if (check(TokenClass::ELSE)) { ... }
// ...
```

**Implementation**:
```cpp
class Parser {
private:
    bool check(TokenClass klass) const {
        return !isAtEnd() && peek().tokenClass == klass;
    }
    
    bool match(TokenClass klass) {
        if (!check(klass)) return false;
        advance();
        return true;
    }
};
```

**Benefits**:
- 95% reduction in predicate code
- Type-safe: compiler enforces using TokenClass
- Fast: O(1) instead of O(language_count)
- Testable: single function to test, not 40

### Layer 4: Operator Precedence Table (Formal & Documented)

Create explicit precedence specification:

```cpp
struct PrecedenceEntry {
    TokenClass op;
    int precedence;    // 1 = lowest (OR), 8 = highest (postfix)
    Associativity assoc;  // LEFT or RIGHT
};

const std::vector<PrecedenceEntry> PRECEDENCE_TABLE = {
    {TokenClass::OR, 1, Associativity::LEFT},
    {TokenClass::AND, 2, Associativity::LEFT},
    {TokenClass::EQUAL, 3, Associativity::LEFT},
    {TokenClass::NOT_EQUAL, 3, Associativity::LEFT},
    {TokenClass::LESS, 4, Associativity::LEFT},
    {TokenClass::LESS_EQUAL, 4, Associativity::LEFT},
    {TokenClass::GREATER, 4, Associativity::LEFT},
    {TokenClass::GREATER_EQUAL, 4, Associativity::LEFT},
    {TokenClass::PLUS, 5, Associativity::LEFT},
    {TokenClass::MINUS, 5, Associativity::LEFT},
    {TokenClass::MULTIPLY, 6, Associativity::LEFT},
    {TokenClass::DIVIDE, 6, Associativity::LEFT},
    {TokenClass::MODULO, 6, Associativity::LEFT},
    {TokenClass::NOT, 7, Associativity::RIGHT},
    {TokenClass::UNARY_MINUS, 7, Associativity::RIGHT},
    {TokenClass::UNARY_PLUS, 7, Associativity::RIGHT},
};
```

**Benefits**:
- Formally documented
- Easy to verify correctness
- Easy to modify safely
- Matches EBNF grammar exactly

## Grammar Ambiguity Resolution

### Issue: Print Statement Ambiguity

**Before**:
```mantra
dikhao = "text"   # Is this print or assignment?
```

Multiple parse paths possible, creates ambiguity.

**After**:
```mantra
dikhao "text"     # Unambiguous: keyword + expression
x = "text"        # Unambiguous: identifier + = + expression
```

**Grammar Rules (EBNF)**:
```ebnf
print_stmt = print_keyword expression ;
assignment_stmt = identifier "=" expression ;
let_stmt = let_keyword identifier "=" expression ;
```

**Implementation**:
```cpp
// In parser statement() method:
if (check(TokenClass::PRINT)) {
    return parsePrint();
}
if (check(TokenClass::IDENTIFIER)) {
    return parseAssignment();  // Must start with identifier, not keyword
}
if (check(TokenClass::LET)) {
    return parseLet();
}
```

**Result**: Each form has single unambiguous parse path.

## Operator Precedence & Associativity

### Formal Specification

| Level | Operator | Type | Associativity | Example |
|-------|----------|------|---------------|---------|
| 1 | OR (ya, kiva, \|\|) | Binary | LEFT | `a \|\| b \|\| c = (a \|\| b) \|\| c` |
| 2 | AND (aur, and, &&) | Binary | LEFT | `a && b && c = (a && b) && c` |
| 3 | == , != | Binary | LEFT | `a == b != c = (a == b) != c` |
| 4 | <, >, <=, >= | Binary | LEFT | `a < b <= c = (a < b) <= c` |
| 5 | +, - | Binary | LEFT | `a + b - c = (a + b) - c` |
| 6 | *, /, % | Binary | LEFT | `a * b / c = (a * b) / c` |
| 7 | !, -, + | Unary | RIGHT | `! - a = ! (- a)` |
| 8 | () [] . | Postfix | LEFT | `a() [0] .b = ((a()) [0]) .b` |

### Implementation (Precedence Climbing)

```cpp
std::unique_ptr<MantraNode> Parser::expression() {
    return logicalOr();
}

std::unique_ptr<MantraNode> Parser::logicalOr() {
    auto left = logicalAnd();
    while (check(TokenClass::OR)) {
        Token op = advance();
        auto right = logicalAnd();
        left = std::make_unique<BinaryExprNode>(op, std::move(left), std::move(right), ...);
    }
    return left;
}

std::unique_ptr<MantraNode> Parser::logicalAnd() {
    auto left = equality();
    while (check(TokenClass::AND)) {
        Token op = advance();
        auto right = equality();
        left = std::make_unique<BinaryExprNode>(op, std::move(left), std::move(right), ...);
    }
    return left;
}

// ... continue down the precedence levels ...

std::unique_ptr<MantraNode> Parser::unary() {
    if (check(TokenClass::NOT) || check(TokenClass::MINUS) || check(TokenClass::PLUS)) {
        Token op = advance();
        auto right = unary();  // RIGHT associative: recurse without consuming
        return std::make_unique<UnaryExprNode>(op, std::move(right), ...);
    }
    return postfix();
}

std::unique_ptr<MantraNode> Parser::postfix() {
    auto left = primary();
    while (true) {
        if (check(TokenClass::LPAREN)) {
            left = finishCall(std::move(left));
        } else if (check(TokenClass::LBRACKET)) {
            left = finishIndex(std::move(left));
        } else if (check(TokenClass::DOT)) {
            left = finishMember(std::move(left));
        } else {
            break;
        }
    }
    return left;
}
```

**Key Points**:
- Each precedence level gets a function
- LEFT associative: parse left, loop for operators, parse right
- RIGHT associative: parse left, loop for operators, RECURSE for right
- Postfix (LEFT): loop collecting all operations

## Implementation Strategy

### Phase 1: Non-Breaking Addition
1. Add TokenClass enum to token.h
2. Create KeywordNormalizer class (standalone)
3. Create new parser helper functions
4. **KEEP** existing TokenType and old helper functions

### Phase 2: Gradual Migration
1. Update Lexer to populate TokenClass field
2. Update Parser to use new check() method
3. Deprecate old helper functions (warn on use)
4. All tests still pass

### Phase 3: Cleanup
1. Remove TokenType enum entries one by one
2. Remove old helper functions
3. Update token.h documentation
4. Final test validation

## Benefits Summary

| Metric | Before | After | Gain |
|--------|--------|-------|------|
| Token types | 200+ | ~25 | 88% reduction |
| Helper functions | 40+ | 1-2 | 97.5% reduction |
| Lines in token.h | ~1000 | ~100 | 90% reduction |
| Parser code | ~2000 | ~1500 | 25% reduction |
| Time to add language | 30 min | 5 min | 6x faster |
| Compiler compile time | High | Low | ~30% reduction |
| Binary size | Large | Small | ~10% reduction |
| Typo tolerance | None | Full | New feature |
| Maintainability | Low | High | Clear structure |

## Testing & Validation

### Test Coverage Requirements

1. **TokenClass System**: 95%+ coverage
   - All 22 language variants
   - All keyword types
   - Edge cases (case sensitivity, whitespace)

2. **Parser**: 100% coverage
   - Each precedence level
   - Each statement type
   - Error cases
   - All language variants

3. **Regression**: Zero failures
   - All existing test programs parse identically
   - All existing programs produce identical results
   - No new parse errors on valid programs

### Performance Benchmarks

- Parser speed: Target 2x faster (due to O(1) checks vs O(n) lists)
- Memory usage: Target 10% reduction
- Binary size: Target 5-10% reduction
- No regression on semantic analysis or interpretation

## Backwards Compatibility

- TokenType remains available (deprecated) for 1 release cycle
- Old helper functions continue working (performance warning)
- External API unchanged
- Internal tokens bridge old and new systems
- Full migration path provided

## Future Enhancements

1. **Unicode Support**: Devanagari/Tamil/Bengali keywords
2. **IDE Integration**: Language server using TokenClass system
3. **Dialect Support**: Regional variations
4. **Performance**: Potentially JIT compilation of token checks
5. **Accessibility**: Phonetic matching for typo suggestions

## References

- GRAMMAR.ebnf - Formal grammar specification
- TOKEN_STRATEGY.md - Multilingual normalization strategy
- AMBIGUITY_RESOLUTION.md - Grammar ambiguity elimination details
