#include "unicode.h"
#include <cctype>
#include <algorithm>

namespace mantra {

std::string Unicode::encodeUTF8(uint32_t codepoint) {
    std::string result;
    
    if (codepoint <= 0x7F) {
        // Single byte (ASCII)
        result += static_cast<char>(codepoint);
    } else if (codepoint <= 0x7FF) {
        // Two bytes
        result += static_cast<char>(0xC0 | (codepoint >> 6));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0xFFFF) {
        // Three bytes
        result += static_cast<char>(0xE0 | (codepoint >> 12));
        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0x10FFFF) {
        // Four bytes
        result += static_cast<char>(0xF0 | (codepoint >> 18));
        result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
    
    return result;
}

bool Unicode::isUTF8ContinuationByte(unsigned char byte) {
    return (byte & 0xC0) == 0x80;
}

int Unicode::getUTF8SequenceLength(unsigned char first_byte) {
    if ((first_byte & 0x80) == 0) return 1;           // 0xxxxxxx
    if ((first_byte & 0xE0) == 0xC0) return 2;        // 110xxxxx
    if ((first_byte & 0xF0) == 0xE0) return 3;        // 1110xxxx
    if ((first_byte & 0xF8) == 0xF0) return 4;        // 11110xxx
    return 0; // Invalid
}

uint32_t Unicode::decodeUTF8(const std::string& str, size_t& pos) {
    if (pos >= str.length()) {
        return 0;
    }
    
    unsigned char first_byte = static_cast<unsigned char>(str[pos]);
    int seq_length = getUTF8SequenceLength(first_byte);
    
    if (seq_length == 0 || pos + seq_length > str.length()) {
        pos++;
        return 0xFFFD; // Replacement character
    }
    
    uint32_t codepoint = 0;
    
    if (seq_length == 1) {
        codepoint = first_byte;
    } else if (seq_length == 2) {
        codepoint = (first_byte & 0x1F) << 6;
        codepoint |= (static_cast<unsigned char>(str[pos + 1]) & 0x3F);
    } else if (seq_length == 3) {
        codepoint = (first_byte & 0x0F) << 12;
        codepoint |= (static_cast<unsigned char>(str[pos + 1]) & 0x3F) << 6;
        codepoint |= (static_cast<unsigned char>(str[pos + 2]) & 0x3F);
    } else if (seq_length == 4) {
        codepoint = (first_byte & 0x07) << 18;
        codepoint |= (static_cast<unsigned char>(str[pos + 1]) & 0x3F) << 12;
        codepoint |= (static_cast<unsigned char>(str[pos + 2]) & 0x3F) << 6;
        codepoint |= (static_cast<unsigned char>(str[pos + 3]) & 0x3F);
    }
    
    pos += seq_length;
    return codepoint;
}

bool Unicode::isValidUTF8(const std::string& str) {
    for (size_t i = 0; i < str.length(); ) {
        unsigned char byte = static_cast<unsigned char>(str[i]);
        int seq_length = getUTF8SequenceLength(byte);
        
        if (seq_length == 0) {
            return false; // Invalid first byte
        }
        
        if (i + seq_length > str.length()) {
            return false; // Incomplete sequence
        }
        
        // Check continuation bytes
        for (int j = 1; j < seq_length; ++j) {
            if (!isUTF8ContinuationByte(static_cast<unsigned char>(str[i + j]))) {
                return false;
            }
        }
        
        i += seq_length;
    }
    return true;
}

bool Unicode::isWhitespace(uint32_t codepoint) {
    // ASCII spaces
    if (codepoint == 0x20 || codepoint == 0x09 || codepoint == 0x0A || 
        codepoint == 0x0D || codepoint == 0x0B) {
        return true;
    }
    // Unicode spaces
    if (codepoint == 0x00A0 || codepoint == 0x1680 || codepoint == 0x2000 ||
        codepoint == 0x2001 || codepoint == 0x2002 || codepoint == 0x2003 ||
        codepoint == 0x2004 || codepoint == 0x2005 || codepoint == 0x2006 ||
        codepoint == 0x2007 || codepoint == 0x2008 || codepoint == 0x2009 ||
        codepoint == 0x200A || codepoint == 0x202F || codepoint == 0x205F ||
        codepoint == 0x3000) {
        return true;
    }
    return false;
}

bool Unicode::isDigit(uint32_t codepoint) {
    // ASCII digits
    if (codepoint >= 0x30 && codepoint <= 0x39) {
        return true;
    }
    
    // Devanagari digits (U+0966 to U+096F)
    if (codepoint >= 0x0966 && codepoint <= 0x096F) {
        return true;
    }
    
    // Bengali digits (U+09E6 to U+09EF)
    if (codepoint >= 0x09E6 && codepoint <= 0x09EF) {
        return true;
    }
    
    // Tamil digits (U+0BE6 to U+0BEF)
    if (codepoint >= 0x0BE6 && codepoint <= 0x0BEF) {
        return true;
    }
    
    // Telugu digits (U+0C66 to U+0C6F)
    if (codepoint >= 0x0C66 && codepoint <= 0x0C6F) {
        return true;
    }
    
    // Kannada digits (U+0CE6 to U+0CEF)
    if (codepoint >= 0x0CE6 && codepoint <= 0x0CEF) {
        return true;
    }
    
    // Gujarati digits (U+0AE6 to U+0AEF)
    if (codepoint >= 0x0AE6 && codepoint <= 0x0AEF) {
        return true;
    }
    
    // Punjabi digits (U+0A66 to U+0A6F)
    if (codepoint >= 0x0A66 && codepoint <= 0x0A6F) {
        return true;
    }
    
    // Malayalam digits (U+0D58 to U+0D5E)
    if (codepoint >= 0x0D58 && codepoint <= 0x0D5E) {
        return true;
    }
    
    // Oriya digits (U+0B66 to U+0B6F)
    if (codepoint >= 0x0B66 && codepoint <= 0x0B6F) {
        return true;
    }
    
    return false;
}

bool Unicode::isAlpha(uint32_t codepoint) {
    // ASCII letters
    if ((codepoint >= 0x41 && codepoint <= 0x5A) ||
        (codepoint >= 0x61 && codepoint <= 0x7A) ||
        codepoint == 0x5F) { // underscore
        return true;
    }
    return false;
}

bool Unicode::isAlphaNumeric(uint32_t codepoint) {
    return isAlpha(codepoint) || isDigit(codepoint);
}

// Script range checks (Unicode ranges)
bool Unicode::isDevanagari(uint32_t codepoint) {
    return codepoint >= 0x0900 && codepoint <= 0x097F;
}

bool Unicode::isTamil(uint32_t codepoint) {
    return codepoint >= 0x0B80 && codepoint <= 0x0BFF;
}

bool Unicode::isTelugu(uint32_t codepoint) {
    return codepoint >= 0x0C00 && codepoint <= 0x0C7F;
}

bool Unicode::isPunjabi(uint32_t codepoint) {
    return codepoint >= 0x0A00 && codepoint <= 0x0A7F;
}

bool Unicode::isBengali(uint32_t codepoint) {
    return codepoint >= 0x0980 && codepoint <= 0x09FF;
}

bool Unicode::isGujarati(uint32_t codepoint) {
    return codepoint >= 0x0A80 && codepoint <= 0x0AFF;
}

bool Unicode::isOriya(uint32_t codepoint) {
    return codepoint >= 0x0B00 && codepoint <= 0x0B7F;
}

bool Unicode::isMalayalam(uint32_t codepoint) {
    return codepoint >= 0x0D00 && codepoint <= 0x0D7F;
}

bool Unicode::isKannada(uint32_t codepoint) {
    return codepoint >= 0x0C80 && codepoint <= 0x0CFF;
}

uint32_t Unicode::toLowerRoman(uint32_t codepoint) {
    // Convert uppercase ASCII to lowercase
    if (codepoint >= 0x41 && codepoint <= 0x5A) {
        return codepoint + 0x20;
    }
    return codepoint;
}

uint32_t Unicode::toUpperRoman(uint32_t codepoint) {
    // Convert lowercase ASCII to uppercase
    if (codepoint >= 0x61 && codepoint <= 0x7A) {
        return codepoint - 0x20;
    }
    return codepoint;
}

std::string Unicode::toLowerString(const std::string& str) {
    std::string result;
    
    for (size_t i = 0; i < str.length(); ) {
        uint32_t codepoint = decodeUTF8(str, i);
        uint32_t lower_codepoint = toLowerRoman(codepoint);
        result += encodeUTF8(lower_codepoint);
    }
    
    return result;
}

size_t Unicode::getUTF8CharCount(const std::string& str) {
    size_t count = 0;
    for (size_t i = 0; i < str.length(); ) {
        decodeUTF8(str, i);
        count++;
    }
    return count;
}

std::string Unicode::getUTF8Substring(const std::string& str, size_t start, size_t length) {
    std::string result;
    size_t char_count = 0;
    size_t bytes_added = 0;
    
    for (size_t i = 0; i < str.length() && bytes_added < length; ) {
        size_t old_pos = i;
        decodeUTF8(str, i);  // Decode to advance position
        
        if (char_count >= start) {
            result += str.substr(old_pos, i - old_pos);
            bytes_added += (i - old_pos);
        }
        
        char_count++;
    }
    
    return result;
}

} // namespace mantra
