# MANTRA Grammar System - Developer's Guide

This guide explains how to use the new TokenClass system, KeywordNormalizer, and formal grammar specification in MANTRA development.

## Quick Start

### Understanding TokenClass

TokenClass is the canonical classification system for tokens in MANTRA. Unlike TokenType (which has 200+ entries), TokenClass has ~25 entries representing semantic categories.

```cpp
#include "core/token_class.h"

using namespace mantra;

// TokenClass categories
TokenClass print_token = TokenClass::PRINT;        // All print keywords
TokenClass if_token = TokenClass::IF;              // All if keywords
TokenClass plus_token = TokenClass::PLUS;          // Addition operator
TokenClass lparen = TokenClass::LPAREN;            // Left parenthesis
```

### Using KeywordNormalizer

The KeywordNormalizer maps all 22 language variants + English to canonical TokenClass:

```cpp
#include "core/keyword_normalizer.h"

using namespace mantra;

KeywordNormalizer normalizer;

// Normalize keywords to canonical forms
TokenClass klass1 = normalizer.normalize("dikhao");    // Hinglish → PRINT
TokenClass klass2 = normalizer.normalize("kaado");     // Tanglish → PRINT
TokenClass klass3 = normalizer.normalize("agar");      // Hinglish → IF
TokenClass klass4 = normalizer.normalize("endral");    // Tanglish → IF

// They all normalize to the same TokenClass
assert(klass1 == klass2 == TokenClass::PRINT);
assert(klass3 == klass4 == TokenClass::IF);
```

### Language Detection

Detect which Indian language a keyword belongs to:

```cpp
UserLanguage lang1 = normalizer.detectLanguage("dikhao");  // Hinglish
UserLanguage lang2 = normalizer.detectLanguage("kaado");   // Tanglish
UserLanguage lang3 = normalizer.detectLanguage("print");   // English

// Check against enum values
if (lang1 == UserLanguage::Hinglish) {
    std::cout << "Hindi/English mix detected" << std::endl;
}
```

### Fuzzy Matching & Error Messages

Get suggestions for misspelled keywords:

```cpp
// Try fuzzy matching
auto match = normalizer.fuzzyMatch("dikho");      // Misspelled

if (match.has_value()) {
    std::string suggestion = normalizer.getSuggestion("dikho");
    std::cout << "Unknown keyword: " << suggestion << std::endl;
    // Output: "Did you mean 'dikhao'?"
}
```

## Grammar Specification

The formal EBNF grammar is in `docs/GRAMMAR.ebnf`. Key sections:

### Operator Precedence

Use TokenClass with getPrecedence() to determine operator precedence:

```cpp
#include "core/token_class.h"

using namespace mantra;

int plus_prec = getPrecedence(TokenClass::PLUS);        // Level 5
int multiply_prec = getPrecedence(TokenClass::MULTIPLY); // Level 6
int and_prec = getPrecedence(TokenClass::AND);          // Level 2
int or_prec = getPrecedence(TokenClass::OR);            // Level 1

// Higher number = higher precedence (binds tighter)
assert(multiply_prec > plus_prec);       // * binds before +
assert(and_prec > or_prec);              // && binds before ||
```

### Associativity

Get operator associativity for proper parsing:

```cpp
#include "core/token_class.h"

using namespace mantra;

Associativity assoc1 = getAssociativity(TokenClass::PLUS);   // LEFT
Associativity assoc2 = getAssociativity(TokenClass::NOT);    // RIGHT

// LEFT associative: a + b + c = (a + b) + c
// RIGHT associative: !a = !(a)
```

## Using TokenClass in Parser

### New Parser Method: checkClass()

The Parser now has a checkClass() method for unified token checking:

```cpp
// In parser.cpp
bool Parser::parseStatement() {
    // OLD WAY (still works, 18+ helper functions)
    if (isPrintKeyword(peek().type)) {
        return parsePrint();
    }
    
    // NEW WAY (recommended for new code)
    if (checkClass(TokenClass::PRINT)) {
        return parsePrint();
    }
    
    // Both are equivalent
}
```

### Migrating Parser Code

To migrate parser code from TokenType to TokenClass:

1. **Identify keyword checking code**:
   ```cpp
   // Find calls like:
   isPrintKeyword(), isIfKeyword(), isAndKeyword(), etc.
   ```

2. **Replace with checkClass()**:
   ```cpp
   // Before
   if (isPrintKeyword(peek().type)) { ... }
   if (isIfKeyword(peek().type)) { ... }
   if (isAndKeyword(peek().type)) { ... }
   
   // After
   if (checkClass(TokenClass::PRINT)) { ... }
   if (checkClass(TokenClass::IF)) { ... }
   if (checkClass(TokenClass::AND)) { ... }
   ```

3. **Test thoroughly** to ensure identical behavior

### Example: Parsing If Statements

```cpp
std::unique_ptr<MantraNode> Parser::parseIf() {
    // Using new checkClass() method
    consume(TokenClass::IF);    // Consumes any IF keyword variant
    
    auto condition = expression();
    
    if (checkClass(TokenClass::THEN)) {
        advance();  // Optional THEN keyword
    }
    
    auto then_body = parseSuite();
    std::unique_ptr<MantraNode> else_body = nullptr;
    
    if (checkClass(TokenClass::ELSE)) {
        advance();
        else_body = parseSuite();
    }
    
    return std::make_unique<IfStmtNode>(
        std::move(condition),
        std::move(then_body),
        std::move(else_body)
    );
}
```

## Adding New Keywords

### Step 1: Add to KEYWORD_MAP

Edit `core/keyword_normalizer.cpp` and add your keyword to the KEYWORD_MAP:

```cpp
const std::unordered_map<std::string, TokenClass> KeywordNormalizer::KEYWORD_MAP = {
    // ... existing entries ...
    
    // Your new keyword (when adding)
    {"mera_keyword", TokenClass::MY_CATEGORY},
};
```

### Step 2: Update TokenClass (if needed)

Add new TokenClass entry to `core/token_class.h`:

```cpp
enum class TokenClass {
    // ... existing entries ...
    MY_CATEGORY,  // New category
    // ...
};
```

### Step 3: Update Helper Functions

Add string conversion to `core/token_class.h`:

```cpp
inline std::string tokenClassToString(TokenClass klass) {
    switch (klass) {
        // ... existing cases ...
        case TokenClass::MY_CATEGORY: return "MY_CATEGORY";
        // ...
    }
}
```

### Step 4: Add Tests

Update `tests/test_keyword_normalizer.cpp`:

```cpp
void test_my_category() {
    KeywordNormalizer normalizer;
    assert(normalizer.normalize("mera_keyword") == TokenClass::MY_CATEGORY);
    std::cout << "✓ MY_CATEGORY keywords work" << std::endl;
}
```

## Understanding the Multilingual Architecture

### Language Map

MANTRA supports keywords in 22 Indian languages + English. Each keyword belongs to one language:

```
HINGLISH (Hindi + English)
├── Print: dikhao
├── If: agar
├── And: aur
└── ...

TANGLISH (Tamil + English)
├── Print: kaado
├── If: endral
├── And: mattrum
└── ...

ENGLISH
├── Print: print
├── If: if
├── And: and
└── ...

(19 more languages...)
```

### Language Detection Logic

Keywords are grouped by language characteristics in KeywordNormalizer:

```cpp
// From keyword_normalizer.cpp
UserLanguage KeywordNormalizer::detectLanguage(const std::string& keyword) {
    if (keyword == "dikhao" || keyword == "agar" || keyword == "aur") {
        return UserLanguage::Hinglish;
    }
    if (keyword == "kaado" || keyword == "endral" || keyword == "mattrum") {
        return UserLanguage::Tanglish;
    }
    // ... (similar for 20 more languages)
    return UserLanguage::English;
}
```

## Performance Considerations

### Keyword Lookup (O(1))

```cpp
// Static initialization happens once
auto normalized = normalizer.normalize("dikhao");  // O(1) hash lookup
```

### Fuzzy Matching (O(n*m))

```cpp
// Only called on keyword lookup failure
auto suggestion = normalizer.fuzzyMatch("dikho");  // O(n*m) Levenshtein
```

**Recommendation**: Don't use fuzzy matching in tight loops; use for error reporting only.

### Parser Token Checking

```cpp
// Old: Multiple if statements in each helper function
if (isPrintKeyword(type)) { ... }  // Linear search through 20 keywords

// New: Direct hash lookup through TokenClass
if (checkClass(TokenClass::PRINT)) { ... }  // O(1) token type check
```

## Debugging & Troubleshooting

### Verify Keyword Mapping

```cpp
KeywordNormalizer normalizer;
TokenClass klass = normalizer.normalize("dikhao");

std::cout << tokenClassToString(klass) << std::endl;
// Output: "PRINT"
```

### Check Language Detection

```cpp
UserLanguage lang = normalizer.detectLanguage("dikhao");
std::string lang_name = userLanguageToString(lang);  // If available
std::cout << "Detected: " << lang_name << std::endl;
```

### Enable Detailed Logging

Lexer now logs warnings for unknown keywords:

```
Warning: Unknown keyword 'dikho' at line 5, column 10. Did you mean 'dikhao'?
```

### Verify Test Coverage

Run the dedicated test suite:

```bash
./build/test_keyword_normalizer
```

Expected output shows all 22 languages tested:
```
✓ PRINT keywords work
✓ IF keywords work
✓ AND keywords work
... (all language variants verified)
```

## Best Practices

### 1. Use TokenClass in New Code

✅ Do this:
```cpp
if (checkClass(TokenClass::PRINT)) { ... }
```

❌ Don't do this:
```cpp
if (isPrintKeyword(peek().type)) { ... }
```

### 2. Leverage Language Detection

✅ Do this:
```cpp
UserLanguage lang = normalizer.detectLanguage(keyword);
// Handle language-specific behavior
```

❌ Don't do this:
```cpp
if (keyword == "dikhao" || keyword == "kaado" || ...) { ... }
```

### 3. Provide Helpful Error Messages

✅ Do this:
```cpp
auto suggestion = normalizer.getSuggestion(misspelled);
reportError("Unknown keyword: " + suggestion);
```

❌ Don't do this:
```cpp
reportError("Unknown keyword: " + misspelled);
```

### 4. Test All Languages

✅ Do this:
```cpp
// Test representative keywords from each language
for (auto& keyword : all_language_variants) {
    auto klass = normalizer.normalize(keyword);
    assert(klass == expected_class);
}
```

❌ Don't do this:
```cpp
// Only test English
assert(normalizer.normalize("print") == TokenClass::PRINT);
```

### 5. Cache Normalizer Instance

✅ Do this:
```cpp
// Static initialization (happens once)
static KeywordNormalizer normalizer;
auto klass = normalizer.normalize(keyword);
```

❌ Don't do this:
```cpp
// Creates new instance for every keyword!
KeywordNormalizer norm;
auto klass = norm.normalize(keyword);
```

## References

- **Grammar Specification**: `docs/GRAMMAR.ebnf`
- **Token Strategy**: `docs/TOKEN_STRATEGY.md`
- **Parser Design**: `docs/PARSER_DESIGN.md`
- **Implementation Report**: `docs/IMPLEMENTATION_REPORT.md`
- **Test Suite**: `tests/test_keyword_normalizer.cpp`

## Contributing

### Adding Support for New Indian Language

1. Add keywords to KEYWORD_MAP in `core/keyword_normalizer.cpp`
2. Add UserLanguage enum entry in `include/mantra/core/types.h`
3. Update language detection logic in `detectLanguage()`
4. Add test cases in `tests/test_keyword_normalizer.cpp`
5. Update documentation in `docs/TOKEN_STRATEGY.md`

### Improving Fuzzy Matching

Current implementation uses Levenshtein distance. Possible improvements:
- Phonetic similarity (similar sounds)
- Common typo patterns
- Keyboard proximity
- Language-specific patterns

### Parser Refactoring

To complete parser refactoring:
1. Replace remaining `isPrintKeyword()` calls with `checkClass(TokenClass::PRINT)`
2. Remove old helper functions after verification
3. Update tests
4. Run performance benchmarks

## Support & Questions

- See test suite examples: `tests/test_keyword_normalizer.cpp`
- Review implementation: `core/keyword_normalizer.cpp`
- Check grammar spec: `docs/GRAMMAR.ebnf`
- Study parser code: `core/parser.cpp`
