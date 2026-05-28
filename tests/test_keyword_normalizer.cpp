#include <iostream>
#include <cassert>
#include <string>
#include "../core/keyword_normalizer.h"
#include "../core/token_class.h"

using namespace mantra;

void test_keyword_normalization() {
    KeywordNormalizer normalizer;
    
    // Test PRINT keywords from all languages
    std::cout << "Testing PRINT keywords..." << std::endl;
    assert(normalizer.normalize("dikhao") == TokenClass::PRINT);  // Hinglish
    assert(normalizer.normalize("kaado") == TokenClass::PRINT);   // Tanglish
    assert(normalizer.normalize("dakho") == TokenClass::PRINT);   // Punglish
    assert(normalizer.normalize("dekhao") == TokenClass::PRINT);  // Benglish
    assert(normalizer.normalize("print") == TokenClass::PRINT);   // English
    std::cout << "✓ PRINT keywords work" << std::endl;
    
    // Test IF keywords
    std::cout << "Testing IF keywords..." << std::endl;
    assert(normalizer.normalize("agar") == TokenClass::IF);       // Hinglish
    assert(normalizer.normalize("endral") == TokenClass::IF);     // Tanglish
    assert(normalizer.normalize("jekr") == TokenClass::IF);       // Punglish
    assert(normalizer.normalize("if") == TokenClass::IF);         // English
    std::cout << "✓ IF keywords work" << std::endl;
    
    // Test AND keywords
    std::cout << "Testing AND keywords..." << std::endl;
    assert(normalizer.normalize("aur") == TokenClass::AND);       // Hinglish
    assert(normalizer.normalize("mattrum") == TokenClass::AND);   // Tanglish
    assert(normalizer.normalize("and") == TokenClass::AND);       // English
    std::cout << "✓ AND keywords work" << std::endl;
    
    // Test OR keywords
    std::cout << "Testing OR keywords..." << std::endl;
    assert(normalizer.normalize("ya") == TokenClass::OR);         // Hinglish
    assert(normalizer.normalize("allathu") == TokenClass::OR);    // Tanglish
    assert(normalizer.normalize("or") == TokenClass::OR);         // English
    std::cout << "✓ OR keywords work" << std::endl;
    
    // Test TRUE/FALSE keywords
    std::cout << "Testing TRUE/FALSE keywords..." << std::endl;
    assert(normalizer.normalize("sach") == TokenClass::TRUE);     // Hinglish true
    assert(normalizer.normalize("jhooth") == TokenClass::FALSE);  // Hinglish false
    assert(normalizer.normalize("true") == TokenClass::TRUE);     // English
    assert(normalizer.normalize("false") == TokenClass::FALSE);   // English
    std::cout << "✓ TRUE/FALSE keywords work" << std::endl;
    
    // Test FUNCTION keywords
    std::cout << "Testing FUNCTION keywords..." << std::endl;
    assert(normalizer.normalize("kaam") == TokenClass::FUNCTION); // Hinglish
    assert(normalizer.normalize("seyal") == TokenClass::FUNCTION); // Tanglish
    assert(normalizer.normalize("function") == TokenClass::FUNCTION); // English
    std::cout << "✓ FUNCTION keywords work" << std::endl;
    
    // Test RETURN keywords
    std::cout << "Testing RETURN keywords..." << std::endl;
    assert(normalizer.normalize("wapas") == TokenClass::RETURN); // Hinglish
    assert(normalizer.normalize("thirumbu") == TokenClass::RETURN); // Tanglish
    assert(normalizer.normalize("return") == TokenClass::RETURN); // English
    std::cout << "✓ RETURN keywords work" << std::endl;
    
    // Test WHILE keywords
    std::cout << "Testing WHILE keywords..." << std::endl;
    assert(normalizer.normalize("baarbaar") == TokenClass::WHILE); // Hinglish (for loop)
    assert(normalizer.normalize("marumurai") == TokenClass::WHILE); // Tanglish
    assert(normalizer.normalize("while") == TokenClass::WHILE); // English
    std::cout << "✓ WHILE keywords work" << std::endl;
    
    std::cout << "✓ All keyword normalization tests passed!" << std::endl;
}

void test_fuzzy_matching() {
    KeywordNormalizer normalizer;
    
    std::cout << "\nTesting fuzzy matching..." << std::endl;
    
    // Test typo tolerance
    auto result = normalizer.fuzzyMatch("dikhao");  // Exact match
    assert(result.has_value());
    assert(result.value() == TokenClass::PRINT);
    
    // Try a slightly misspelled word
    auto suggestion = normalizer.getSuggestion("dikho");  // Missing 'a'
    std::cout << "  Suggestion for 'dikho': " << suggestion << std::endl;
    
    std::cout << "✓ Fuzzy matching tests passed!" << std::endl;
}

void test_language_detection() {
    KeywordNormalizer normalizer;
    
    std::cout << "\nTesting language detection..." << std::endl;
    
    // Test language detection
    auto lang1 = normalizer.detectLanguage("dikhao");  // Hinglish
    std::cout << "  Language for 'dikhao': " << (lang1 == UserLanguage::Hinglish ? "Hinglish" : "Other") << std::endl;
    
    auto lang2 = normalizer.detectLanguage("kaado");   // Tanglish
    std::cout << "  Language for 'kaado': " << (lang2 == UserLanguage::Tanglish ? "Tanglish" : "Other") << std::endl;
    
    auto lang3 = normalizer.detectLanguage("print");   // English
    std::cout << "  Language for 'print': " << (lang3 == UserLanguage::English ? "English" : "Other") << std::endl;
    
    std::cout << "✓ Language detection tests passed!" << std::endl;
}

void test_tokenclass_conversion() {
    std::cout << "\nTesting TokenClass to string conversion..." << std::endl;
    
    assert(tokenClassToString(TokenClass::PRINT) == "PRINT");
    assert(tokenClassToString(TokenClass::IF) == "IF");
    assert(tokenClassToString(TokenClass::AND) == "AND");
    assert(tokenClassToString(TokenClass::OR) == "OR");
    assert(tokenClassToString(TokenClass::PLUS) == "PLUS");
    assert(tokenClassToString(TokenClass::MINUS) == "MINUS");
    
    std::cout << "✓ TokenClass to string conversion tests passed!" << std::endl;
}

void test_operator_precedence() {
    std::cout << "\nTesting operator precedence..." << std::endl;
    
    // OR has lower precedence than AND
    assert(getPrecedence(TokenClass::OR) < getPrecedence(TokenClass::AND));
    
    // AND has lower precedence than comparison
    assert(getPrecedence(TokenClass::AND) < getPrecedence(TokenClass::EQUAL));
    
    // Comparison has lower precedence than arithmetic
    assert(getPrecedence(TokenClass::EQUAL) < getPrecedence(TokenClass::PLUS));
    
    // Arithmetic has lower precedence than multiplication
    assert(getPrecedence(TokenClass::PLUS) < getPrecedence(TokenClass::MULTIPLY));
    
    std::cout << "✓ Operator precedence tests passed!" << std::endl;
}

int main() {
    try {
        std::cout << "=== MANTRA Keyword Normalizer and TokenClass Tests ===" << std::endl;
        
        test_keyword_normalization();
        test_fuzzy_matching();
        test_language_detection();
        test_tokenclass_conversion();
        test_operator_precedence();
        
        std::cout << "\n✓ All tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}
