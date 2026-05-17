#ifndef MANTRA_ERROR_H
#define MANTRA_ERROR_H

#include <string>

namespace mantra {

enum class ErrorType {
    SYNTAX_ERROR,
    LEXER_ERROR,
    SEMANTIC_ERROR,
    RUNTIME_ERROR,
    TYPE_ERROR,
    UNKNOWN_IDENTIFIER,
    INVALID_OPERATION,
    FILE_NOT_FOUND,
    IO_ERROR,
    WARNING,
    INFO
};

class ErrorHandler {
public:
    // Print error with Hindi and English messages
    static void printError(ErrorType type, const std::string& message, 
                          int line, int column);
    
    // Print warning (non-fatal)
    static void printWarning(const std::string& message, int line, int column);
    
    // Throw error and exit
    static void throwError(ErrorType type, const std::string& message,
                          int line, int column, int exit_code = 1);
    
    // Get error message in Hindi
    static std::string getHindiMessage(ErrorType type, const std::string& detail);
    
    // Get error message in English
    static std::string getEnglishMessage(ErrorType type, const std::string& detail);
    
    // Print colored output
    static void printColored(const std::string& text, const std::string& color);
    
    // Format location information
    static std::string formatLocation(int line, int column);
    
    // Print error with source context
    static void printErrorWithContext(ErrorType type, const std::string& message,
                                     const std::string& context, int line, int column);
    
    // Error statistics
    static int getErrorCount();
    static int getWarningCount();
    static void resetCounts();
    static void incrementErrorCount();
    static void incrementWarningCount();
    
    // Set verbosity level (0 = quiet, 1 = normal, 2 = verbose)
    static void setVerbosity(int level);
    
    // Get current verbosity level
    static int getVerbosity();

private:
    static int verbosity_level;
    
    // Color codes
    static const std::string COLOR_RED;
    static const std::string COLOR_YELLOW;
    static const std::string COLOR_GREEN;
    static const std::string COLOR_RESET;
};

} // namespace mantra

#endif // MANTRA_ERROR_H
