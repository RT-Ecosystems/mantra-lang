# MANTRA Token Strategy: Canonical Token System & Multilingual Normalization

## Overview

This document defines the token architecture for MANTRA, addressing the current token explosion problem (200+ TokenType entries) by introducing a canonical token system with multilingual keyword normalization.

## Problem Statement

### Current Architecture Issues

1. **Token Explosion**: 200+ separate TokenType enum entries (KW_DIKHAO, KW_KAADO, KW_DAKHO, etc.)
   - Example: One print keyword per language = 19 separate token types
   - Results in massive token.h file and unmaintainable switch statements

2. **Redundant Parsing**: 40+ helper functions with hardcoded token lists
   ```cpp
   bool Parser::isPrintKeyword(TokenType type) const {
       return containsTokenType({
           TokenType::KW_DIKHAO, TokenType::KW_KAADO, TokenType::KW_DAKHO, ...
       }, type);
   }
   ```

3. **Semantic Duplication**: Lexer and parser both contain language-specific keywords

4. **Poor Maintainability**: Adding a new language variant requires:
   - Adding TokenType enum entry
   - Adding to keyword_map in lexer
   - Adding to 5+ helper functions in parser
   - Duplicated code across multiple files

5. **No Typo Tolerance**: Strict keyword matching, no suggestions for misspellings

6. **No Unicode Support**: Only romanized keywords supported

## Solution: Canonical Token System

### Architecture Layers

```
┌─────────────────────────────────────────────────────┐
│ Parser (uses TokenClass)                             │
└──────────────────┬──────────────────────────────────┘
                   │
┌──────────────────▼──────────────────────────────────┐
│ Semantic Analyzer (language detection)               │
└──────────────────┬──────────────────────────────────┘
                   │
┌──────────────────▼──────────────────────────────────┐
│ Lexer (produces canonical tokens + original keyword)│
└──────────────────┬──────────────────────────────────┘
                   │
┌──────────────────▼──────────────────────────────────┐
│ KeywordNormalizer (all 22 languages → canonical)    │
└──────────────────┬──────────────────────────────────┘
                   │
┌──────────────────▼──────────────────────────────────┐
│ Unicode Normalizer (optional: script → romanized)   │
└─────────────────────────────────────────────────────┘
```

### Canonical Token Types

Instead of 200+ tokens, define ~25 canonical TokenClass entries:

```cpp
enum class TokenClass {
    // Statements
    PRINT,          // All print keywords (dikhao, kaado, dakho, ...)
    IF,             // All if keywords (agar, endral, jekr, ...)
    ELSE,           // All else keywords (warna, illena, nahole, ...)
    WHILE,          // All while keywords (baarbaar, marumurai, ...)
    FOR,            // All for keywords (baarbaar, for)
    FUNCTION,       // All function keywords (kaam, seyal, ...)
    RETURN,         // All return keywords (wapas, thirumbu, ...)
    BREAK,          // break
    CONTINUE,       // continue
    LET,            // All let keywords (rakho, let)
    
    // Literals & Identifiers
    TRUE,           // All true keywords (sach, unmai, satya, ...)
    FALSE,          // All false keywords (jhooth, poi, mittha, ...)
    NULL_LIT,       // null
    NUMBER,         // 123, 45.67, 0xFF
    STRING,         // "text", 'text', """text"""
    IDENTIFIER,     // variable names
    
    // Operators
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    EQUAL,          // ==
    NOT_EQUAL,      // !=
    LESS,           // <
    LESS_EQUAL,     // <=
    GREATER,        // >
    GREATER_EQUAL,  // >=
    
    // Logical
    AND,            // All and keywords (aur, and, mattrum, ...)
    OR,             // All or keywords (ya, or, kiva, ...)
    NOT,            // All not keywords (nahi, not, !)
    
    // Assignment & Control
    ASSIGN,         // =
    THEN,           // All then keywords (tab, then)
    FROM,           // All from keywords (se, from)
    TO,             // All to keywords (tak, to)
    
    // Delimiters
    LPAREN,         // (
    RPAREN,         // )
    LBRACE,         // {
    RBRACE,         // }
    LBRACKET,       // [
    RBRACKET,       // ]
    COMMA,          // ,
    DOT,            // .
    SEMICOLON,      // ;
    COLON,          // :
    
    // Special
    NEWLINE,
    EOF_TOKEN,
    UNKNOWN,
    ERROR_TOKEN
};
```

### Keyword Mapping

Create a central mapping from all keyword variants to canonical forms:

```cpp
const std::unordered_map<std::string, TokenClass> KEYWORD_MAP = {
    // Print
    {"dikhao", TokenClass::PRINT},
    {"kaado", TokenClass::PRINT},
    {"dakho", TokenClass::PRINT},
    {"dekhao", TokenClass::PRINT},
    {"batavo", TokenClass::PRINT},
    // ... etc for all 19 print keywords
    {"print", TokenClass::PRINT},
    
    // If
    {"agar", TokenClass::IF},
    {"endral", TokenClass::IF},
    {"jekr", TokenClass::IF},
    // ... etc for all 20 if keywords
    {"if", TokenClass::IF},
    
    // Else
    {"warna", TokenClass::ELSE},
    {"illena", TokenClass::ELSE},
    // ... etc
    
    // And so on for all keyword types
};
```

## KeywordNormalizer Component

### Purpose

Maps arbitrary input keywords to canonical forms with optional typo tolerance.

### Interface

```cpp
class KeywordNormalizer {
public:
    // Normalize a keyword to its canonical token class
    // Returns TokenClass::UNKNOWN if not found
    TokenClass normalize(const std::string& keyword) const;
    
    // Get canonical representative keyword for display/error messages
    // Returns first variant (e.g., "dikhao" for PRINT)
    std::string getCanonicalKeyword(TokenClass klass) const;
    
    // Get all variants for a token class
    std::vector<std::string> getVariants(TokenClass klass) const;
    
    // Fuzzy match: find similar keywords for error suggestions
    // Returns empty if similarity < threshold
    std::optional<TokenClass> fuzzyMatch(const std::string& keyword) const;
    
    // Get suggestion string for error message
    std::string getSuggestion(const std::string& misspelled) const;
    
    // Constructor with language-specific normalization
    KeywordNormalizer(bool enableUnicode = false, bool enableFuzzy = true);
};
```

### Implementation Strategy

1. **Static Lookup Table**: Hash map for O(1) lookups
2. **Trie for Prefix Matching**: Optional, for autocomplete or fuzzy matching
3. **Levenshtein Distance**: For typo tolerance (distance threshold = 1-2)
4. **Common Typo Patterns**: Special cases for common mistakes
   - Vocalic substitutions (a↔e, i↔u, o↔a)
   - Consonant confusion (s↔sh, t↔th, etc.)
   - Doubled consonants (rarar→baarbar)

### Caching

- Cache normalized results to avoid repeated computation
- Thread-safe singleton pattern for shared instance

## Integration Points

### 1. Lexer Integration

**Before** (current):
```cpp
Token Lexer::readIdentifierOrKeyword() {
    std::string ident = readIdentifier();
    auto it = keyword_map.find(ident);
    if (it != keyword_map.end()) {
        return Token(it->second, ident, line, column);
    }
    return Token(TokenType::IDENTIFIER, ident, line, column);
}
```

**After** (with normalization):
```cpp
Token Lexer::readIdentifierOrKeyword() {
    std::string ident = readIdentifier();
    
    // Try exact match first
    TokenClass klass = normalizer.normalize(ident);
    if (klass != TokenClass::UNKNOWN) {
        return Token(klass, ident, line, column);
    }
    
    // Try fuzzy match (with typo suggestion)
    auto fuzzy = normalizer.fuzzyMatch(ident);
    if (fuzzy) {
        // Log suggestion: "Did you mean 'dikhao'?"
        warning_handler.suggestKeyword(ident, normalizer.getSuggestion(ident));
        return Token(fuzzy.value(), ident, line, column);
    }
    
    return Token(TokenClass::IDENTIFIER, ident, line, column);
}
```

### 2. Parser Integration

**Before** (current - manual helper functions):
```cpp
if (isPrintKeyword(peek().type)) {
    return parsePrint();
}

bool Parser::isPrintKeyword(TokenType type) const {
    return containsTokenType({
        TokenType::KW_DIKHAO, TokenType::KW_KAADO, TokenType::KW_DAKHO, ...
    }, type);
}
```

**After** (with TokenClass):
```cpp
if (check(TokenClass::PRINT)) {
    return parsePrint();
}

bool Parser::check(TokenClass klass) const {
    return !isAtEnd() && peek().tokenClass == klass;
}
```

### 3. Token Structure Update

**Current**:
```cpp
struct Token {
    TokenType type;      // 200+ types
    std::string lexeme;
    std::string value;
    int line, column;
};
```

**Updated** (backwards compatible):
```cpp
struct Token {
    TokenType type;          // Keep for backwards compatibility
    TokenClass tokenClass;   // NEW: canonical classification
    std::string lexeme;      // Original keyword (for error messages)
    std::string value;
    int line, column;
};
```

## Language Detection

### Current Approach
- Detects language from lexeme in parser
- Loose, non-deterministic

### Improved Approach
1. **Lexer Phase**: Detects language from first keyword
2. **Semantic Analysis Phase**: Validates language consistency
3. **Error Reporting Phase**: Uses detected language for messages

```cpp
enum class UserLanguage {
    UNKNOWN,
    HINGLISH,    // Hindi + English (agar, dikhao, ...)
    TANGLISH,    // Tamil + English (endral, kaado, ...)
    // ... 20 more
    ENGLISH      // Pure English
};

class LanguageDetector {
public:
    UserLanguage detectFromKeyword(const std::string& keyword) const;
    UserLanguage detectFromProgram(const std::vector<Token>& tokens) const;
    std::string getLanguageName(UserLanguage lang) const;
};
```

## Benefits

| Aspect | Before | After | Improvement |
|--------|--------|-------|-------------|
| Token Types | 200+ | ~25 | 88% reduction |
| Parser Helper Functions | 40+ | 1-2 | 95% reduction |
| Keyword Maintenance | Per-language | Centralized | Single source of truth |
| Typo Tolerance | None | Full | Fuzzy matching with suggestions |
| Unicode Support | Not possible | Possible | Infrastructure ready |
| Parser Complexity | O(language_count) | O(1) | Linear to constant |
| New Language Support | 5+ files to modify | 1 map entry | 90% reduction |

## Migration Path

### Phase 1: Non-Breaking Addition
- Add TokenClass enum (parallel to TokenType)
- Add normalizer component (standalone)
- Both systems coexist initially

### Phase 2: Gradual Migration
- Update lexer to populate both TokenType and TokenClass
- Update parser predicates to use TokenClass
- Keep TokenType for backwards compatibility

### Phase 3: Cleanup
- Remove TokenType entries gradually
- Consolidate to TokenClass-only system
- Full documentation update

## Example: Printing "Hello" in Multiple Languages

### Current Implementation Challenge
- Each language needs separate parsing logic
- Parser has to know all 19 print keywords
- Impossible to extend without modifying parser

### With New System
```cpp
// All these parse identically
dikhao "Hello"      // Hinglish
kaado "Hello"       // Tanglish
dakho "Hello"       // Punglish
dekhao "Hello"      // Benglish
// ... etc

// Parser sees TokenClass::PRINT for all
if (check(TokenClass::PRINT)) {
    auto keyword = advance();
    auto expr = expression();
    // Create PrintStmtNode - same logic for all languages
}
```

### Typo Tolerance
```
User types: "dikho" (missing 'a')
Lexer detects: UNKNOWN → fuzzyMatch()
Suggestion: "Did you mean 'dikhao'?"
Parsed as: TokenClass::PRINT (corrected)
```

## Performance Impact

- **Lexer**: O(1) hash lookup vs O(language_count) sequential matching → ~2x faster
- **Parser**: Single comparison vs 40+ comparisons → ~40x faster predicate checks
- **Memory**: Reduced token enum size (compiler optimization) → slightly smaller binary

## Backwards Compatibility

- Existing TokenType entries remain functional (deprecated)
- New code uses TokenClass
- Semantic analyzer bridges old and new systems
- No breaking changes to external API

## Testing Strategy

1. **Unit Tests**
   - KeywordNormalizer: all 22 languages × all keyword types
   - Typo tolerance: common misspellings
   - Edge cases: partial words, case sensitivity

2. **Integration Tests**
   - Existing programs parse identically
   - Error messages improved
   - Language detection accurate

3. **Regression Tests**
   - No performance degradation
   - No new parse errors on valid programs
   - Suggestions work for all typos

## Future Enhancements

1. **Unicode Support**: Devanagari, Tamil, Bengali scripts
2. **Dialect Support**: Regional variations
3. **Autocomplete**: IDE integration with fuzzy matching
4. **Translation**: Automatic keyword translation
5. **Accessibility**: Phonetic matching for pronunciation-based typos
