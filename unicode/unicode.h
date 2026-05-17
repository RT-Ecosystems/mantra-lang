#ifndef MANTRA_UNICODE_H
#define MANTRA_UNICODE_H

#include <string>
#include <vector>
#include <cstdint>

namespace mantra {

class Unicode {
public:
    // UTF-8 encoding/decoding
    static std::string encodeUTF8(uint32_t codepoint);
    static uint32_t decodeUTF8(const std::string& str, size_t& pos);
    static bool isValidUTF8(const std::string& str);
    
    // Character classification
    static bool isWhitespace(uint32_t codepoint);
    static bool isDigit(uint32_t codepoint);
    static bool isAlpha(uint32_t codepoint);
    static bool isAlphaNumeric(uint32_t codepoint);
    
    // Script detection
    static bool isDevanagari(uint32_t codepoint);
    static bool isTamil(uint32_t codepoint);
    static bool isTelugu(uint32_t codepoint);
    static bool isPunjabi(uint32_t codepoint);
    static bool isBengali(uint32_t codepoint);
    static bool isGujarati(uint32_t codepoint);
    static bool isOriya(uint32_t codepoint);
    static bool isMalayalam(uint32_t codepoint);
    static bool isKannada(uint32_t codepoint);
    
    // Case conversion
    static uint32_t toLowerRoman(uint32_t codepoint);
    static uint32_t toUpperRoman(uint32_t codepoint);
    static std::string toLowerString(const std::string& str);
    
    // UTF-8 utilities
    static size_t getUTF8CharCount(const std::string& str);
    static std::string getUTF8Substring(const std::string& str, size_t start, size_t length);

private:
    // Helper methods
    static bool isUTF8ContinuationByte(unsigned char byte);
    static int getUTF8SequenceLength(unsigned char first_byte);
};

} // namespace mantra

#endif // MANTRA_UNICODE_H
