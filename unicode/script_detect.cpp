// MANTRA Language - Script Detection Implementation
// Detects and analyzes the script type of characters and text
// Supports 9 major Indian scripts plus Roman/Latin script
// Provides confidence scoring and mixed-script detection

#include "script_detect.h"
#include "unicode.h"
#include <algorithm>

namespace mantra {

Script ScriptDetector::detectScriptOfChar(uint32_t codepoint) {
    // Check Roman/ASCII first
    if ((codepoint >= 0x41 && codepoint <= 0x5A) ||  // Uppercase A-Z
        (codepoint >= 0x61 && codepoint <= 0x7A) ||  // Lowercase a-z
        (codepoint >= 0x30 && codepoint <= 0x39)) {  // Digits 0-9
        return Script::ROMAN;
    }
    
    // Common Roman punctuation
    if ((codepoint >= 0x20 && codepoint <= 0x40) ||
        (codepoint >= 0x5B && codepoint <= 0x60) ||
        (codepoint >= 0x7B && codepoint <= 0x7E)) {
        return Script::ROMAN;
    }
    
    // Check Indian scripts
    if (Unicode::isDevanagari(codepoint)) {
        return Script::DEVANAGARI;
    }
    if (Unicode::isTamil(codepoint)) {
        return Script::TAMIL;
    }
    if (Unicode::isTelugu(codepoint)) {
        return Script::TELUGU;
    }
    if (Unicode::isPunjabi(codepoint)) {
        return Script::PUNJABI;
    }
    if (Unicode::isBengali(codepoint)) {
        return Script::BENGALI;
    }
    if (Unicode::isGujarati(codepoint)) {
        return Script::GUJARATI;
    }
    if (Unicode::isOriya(codepoint)) {
        return Script::ORIYA;
    }
    if (Unicode::isMalayalam(codepoint)) {
        return Script::MALAYALAM;
    }
    if (Unicode::isKannada(codepoint)) {
        return Script::KANNADA;
    }
    
    return Script::UNKNOWN;
}

std::map<Script, int> ScriptDetector::countScripts(const std::string& text) {
    std::map<Script, int> counts;
    
    for (size_t i = 0; i < text.length(); ) {
        uint32_t codepoint = Unicode::decodeUTF8(text, i);
        Script script = detectScriptOfChar(codepoint);
        
        // Count only alphanumeric characters
        if (Unicode::isAlphaNumeric(codepoint) || 
            (script != Script::ROMAN && script != Script::UNKNOWN)) {
            counts[script]++;
        }
    }
    
    return counts;
}

Script ScriptDetector::detectScript(const std::string& text) {
    std::map<Script, int> script_counts = countScripts(text);
    
    if (script_counts.empty()) {
        return Script::UNKNOWN;
    }
    
    // Find script with highest count
    Script dominant_script = Script::UNKNOWN;
    int max_count = 0;
    
    for (const auto& pair : script_counts) {
        if (pair.second > max_count) {
            max_count = pair.second;
            dominant_script = pair.first;
        }
    }
    
    // If multiple scripts present, return MIXED
    if (script_counts.size() > 1) {
        return Script::MIXED;
    }
    
    return dominant_script;
}

bool ScriptDetector::isMixedScript(const std::string& text) {
    std::map<Script, int> script_counts = countScripts(text);
    
    // Filter out UNKNOWN and ROMAN punctuation for mixed script detection
    int significant_scripts = 0;
    for (const auto& pair : script_counts) {
        if (pair.first != Script::UNKNOWN && pair.first != Script::ROMAN) {
            significant_scripts++;
        }
    }
    
    // Mixed if we have both ROMAN (for Hinglish, etc.) and an Indian script
    if (significant_scripts > 0 && script_counts.count(Script::ROMAN) > 0) {
        return true;
    }
    
    // Or if we have multiple Indian scripts
    return significant_scripts > 1;
}

Script ScriptDetector::getDominantScript(const std::string& text) {
    std::map<Script, int> script_counts = countScripts(text);
    
    if (script_counts.empty()) {
        return Script::UNKNOWN;
    }
    
    Script dominant_script = Script::UNKNOWN;
    int max_count = 0;
    
    for (const auto& pair : script_counts) {
        if (pair.second > max_count) {
            max_count = pair.second;
            dominant_script = pair.first;
        }
    }
    
    return dominant_script;
}

double ScriptDetector::getDetectionConfidence(const std::string& text) {
    std::map<Script, int> script_counts = countScripts(text);
    
    if (script_counts.empty()) {
        return 0.0;
    }
    
    int total_chars = 0;
    int max_count = 0;
    
    for (const auto& pair : script_counts) {
        total_chars += pair.second;
        max_count = std::max(max_count, pair.second);
    }
    
    if (total_chars == 0) {
        return 0.0;
    }
    
    // Confidence is the ratio of dominant script to total
    return static_cast<double>(max_count) / static_cast<double>(total_chars);
}

std::string ScriptDetector::getScriptName(Script script) {
    switch (script) {
        case Script::UNKNOWN: return "Unknown";
        case Script::ROMAN: return "Roman";
        case Script::DEVANAGARI: return "Devanagari";
        case Script::TAMIL: return "Tamil";
        case Script::TELUGU: return "Telugu";
        case Script::PUNJABI: return "Punjabi";
        case Script::BENGALI: return "Bengali";
        case Script::GUJARATI: return "Gujarati";
        case Script::ORIYA: return "Oriya";
        case Script::MALAYALAM: return "Malayalam";
        case Script::KANNADA: return "Kannada";
        case Script::MIXED: return "Mixed";
        default: return "Unknown";
    }
}

// Helper method to validate script ranges and provide analysis
bool isValidScriptCharacter(uint32_t codepoint) {
    Script script = ScriptDetector::detectScriptOfChar(codepoint);
    return script != Script::UNKNOWN;
}

// Extended detection capabilities
// Checks if script is one of the 9 supported Indian scripts
bool isSupported9IndianScript(Script script) {
    return script == Script::DEVANAGARI || script == Script::TAMIL ||
           script == Script::TELUGU || script == Script::PUNJABI ||
           script == Script::BENGALI || script == Script::GUJARATI ||
           script == Script::ORIYA || script == Script::MALAYALAM ||
           script == Script::KANNADA;
}

// Script detection implementation complete

} // namespace mantra
