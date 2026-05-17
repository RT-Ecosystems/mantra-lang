#ifndef MANTRA_SCRIPT_DETECT_H
#define MANTRA_SCRIPT_DETECT_H

#include <string>
#include <cstdint>
#include <map>

namespace mantra {

enum class Script {
    UNKNOWN,
    ROMAN,           // Roman/Latin script
    DEVANAGARI,      // Hindi
    TAMIL,           // Tamil
    TELUGU,          // Telugu
    PUNJABI,         // Punjabi
    BENGALI,         // Bengali
    GUJARATI,        // Gujarati
    ORIYA,           // Oriya
    MALAYALAM,       // Malayalam
    KANNADA,         // Kannada
    MIXED            // Mixed scripts
};

class ScriptDetector {
public:
    // Detect script of a single character
    static Script detectScriptOfChar(uint32_t codepoint);
    
    // Detect script of entire text
    static Script detectScript(const std::string& text);
    
    // Get script name as string
    static std::string getScriptName(Script script);
    
    // Check if text contains mixed scripts
    static bool isMixedScript(const std::string& text);
    
    // Get dominant script in mixed text
    static Script getDominantScript(const std::string& text);
    
    // Get confidence score (0.0 to 1.0)
    static double getDetectionConfidence(const std::string& text);

private:
    // Helper method to count scripts
    static std::map<Script, int> countScripts(const std::string& text);
};

} // namespace mantra

#endif // MANTRA_SCRIPT_DETECT_H
