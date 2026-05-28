#ifndef MANTRA_KEYWORD_NORMALIZER_H
#define MANTRA_KEYWORD_NORMALIZER_H

#include "token_class.h"
#include "../include/mantra/core/types.h"
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

namespace mantra {

/**
 * Multilingual Keyword Normalization System
 *
 * Maps all keyword variants from 22 Indian languages + English to canonical TokenClass.
 * Supports fuzzy matching for typo tolerance and language detection.
 *
 * Architecture:
 * - Central keyword mapping: all variants → TokenClass
 * - Language detection: keyword → UserLanguage
 * - Typo tolerance: misspelled word → suggestion
 * - Caching: O(1) lookups after first access
 *
 * Example:
 *   normalizer.normalize("dikhao")  → TokenClass::PRINT
 *   normalizer.normalize("kaado")   → TokenClass::PRINT
 *   normalizer.normalize("agar")    → TokenClass::IF
 *   normalizer.fuzzyMatch("dikho")  → TokenClass::PRINT (with suggestion)
 */

class KeywordNormalizer {
public:
    /**
     * Constructor
     * @param enableFuzzy Enable fuzzy matching with typo suggestions (default: true)
     */
    KeywordNormalizer(bool enableFuzzy = true);
    
    /**
     * Normalize a keyword to its canonical TokenClass
     * @param keyword Raw keyword text (case-sensitive)
     * @return TokenClass if found, TokenClass::UNKNOWN otherwise
     *
     * Examples:
     *   "dikhao" → TokenClass::PRINT
     *   "kaado" → TokenClass::PRINT
     *   "agar" → TokenClass::IF
     *   "xyz" → TokenClass::UNKNOWN
     */
    TokenClass normalize(const std::string& keyword) const;
    
    /**
     * Get canonical representative keyword for a TokenClass
     * Useful for error messages and suggestions.
     * 
     * @param klass The TokenClass
     * @return First variant (e.g., "dikhao" for TokenClass::PRINT)
     *         Returns empty string if class has no keywords
     */
    std::string getCanonicalKeyword(TokenClass klass) const;
    
    /**
     * Get all keyword variants for a TokenClass
     * @param klass The TokenClass
     * @return Vector of all variants (e.g., ["dikhao", "kaado", "dakho", ...] for PRINT)
     */
    std::vector<std::string> getVariants(TokenClass klass) const;
    
    /**
     * Fuzzy match: find similar keywords for typo tolerance
     * Uses Levenshtein distance with threshold.
     * 
     * @param keyword Possibly misspelled keyword
     * @return TokenClass of closest match, empty if no match above threshold
     *
     * Examples:
     *   "dikho" (missing 'a') → TokenClass::PRINT
     *   "agar" (exact) → TokenClass::IF
     *   "xyz" (no match) → empty
     */
    std::optional<TokenClass> fuzzyMatch(const std::string& keyword) const;
    
    /**
     * Get user-friendly suggestion for misspelled keyword
     * @param misspelled The misspelled keyword
     * @return Suggestion string for error message
     *
     * Examples:
     *   "dikho" → "Did you mean 'dikhao'?"
     *   "afgr" → "Did you mean 'agar'?"
     *   "xyz" → "(no suggestions)"
     */
    std::string getSuggestion(const std::string& misspelled) const;
    
    /**
     * Detect language from keyword
     * @param keyword The keyword to detect language from
     * @return UserLanguage if keyword is language-specific, ENGLISH otherwise
     *
     * Examples:
     *   "dikhao" → UserLanguage::HINGLISH
     *   "kaado" → UserLanguage::TANGLISH
     *   "print" → UserLanguage::ENGLISH
     */
    UserLanguage detectLanguage(const std::string& keyword) const;
    
    /**
     * Get human-readable language name
     * @param lang The UserLanguage
     * @return Display name (e.g., "Hinglish", "Tanglish")
     */
    static std::string getLanguageName(UserLanguage lang);
    
    /**
     * Check if a keyword is valid (known to normalizer)
     * @param keyword The keyword to check
     * @return true if keyword is known, false otherwise
     */
    bool isValid(const std::string& keyword) const;
    
    /**
     * Get count of all known keywords
     * @return Total number of keywords across all 22 languages + English
     */
    size_t getKeywordCount() const;

private:
    // Central mapping: keyword → TokenClass
    // Static for efficiency (shared across all instances)
    static const std::unordered_map<std::string, TokenClass> KEYWORD_MAP;
    
    // Reverse mapping: TokenClass → list of keywords
    // Built on first use
    mutable std::unordered_map<int, std::vector<std::string>> CLASS_TO_KEYWORDS;
    mutable bool keywords_indexed = false;
    
    // Language detection cache
    mutable std::unordered_map<std::string, UserLanguage> LANGUAGE_CACHE;
    
    // Configuration
    bool enable_fuzzy_matching = true;
    
    // Fuzzy matching threshold (0-100, edit distance percentage)
    // E.g., 30 = allow 30% edit distance
    static constexpr int FUZZY_THRESHOLD = 40;
    
    // Build reverse index for fast class → keywords lookup
    void buildKeywordIndex() const;
    
    // Calculate Levenshtein distance between two strings
    static int levenshteinDistance(const std::string& a, const std::string& b);
    
    // Fuzzy match implementation
    std::optional<TokenClass> fuzzyMatchInternal(const std::string& keyword) const;
};

} // namespace mantra

#endif // MANTRA_KEYWORD_NORMALIZER_H
