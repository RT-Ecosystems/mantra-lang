// MANTRA Language - Error Handler Implementation
// This file implements comprehensive error handling with bilingual support
// Errors are reported in both Hindi and English for better clarity
// ANSI color codes are used for visual formatting in terminals

#include "error.h"
#include <iostream>
#include <cstdlib>
#include <sstream>

namespace mantra {

int ErrorHandler::verbosity_level = 1;

// ANSI color codes
const std::string ErrorHandler::COLOR_RED = "\033[1;31m";
const std::string ErrorHandler::COLOR_YELLOW = "\033[1;33m";
const std::string ErrorHandler::COLOR_GREEN = "\033[1;32m";
const std::string ErrorHandler::COLOR_RESET = "\033[0m";

std::string ErrorHandler::getHindiMessage(ErrorType type, const std::string& detail) {
    switch (type) {
        case ErrorType::SYNTAX_ERROR:
            return "वाक्य विन्यास त्रुटि (Syntax Error): " + detail;
        case ErrorType::LEXER_ERROR:
            return "शब्दविश्लेषक त्रुटि (Lexer Error): " + detail;
        case ErrorType::SEMANTIC_ERROR:
            return "शब्दार्थ त्रुटि (Semantic Error): " + detail;
        case ErrorType::RUNTIME_ERROR:
            return "रन-टाइम त्रुटि (Runtime Error): " + detail;
        case ErrorType::TYPE_ERROR:
            return "प्रकार त्रुटि (Type Error): " + detail;
        case ErrorType::UNKNOWN_IDENTIFIER:
            return "अपरिचित पहचानकर्ता (Unknown Identifier): " + detail;
        case ErrorType::INVALID_OPERATION:
            return "अमान्य संचालन (Invalid Operation): " + detail;
        case ErrorType::FILE_NOT_FOUND:
            return "फाइल नहीं मिली (File Not Found): " + detail;
        case ErrorType::IO_ERROR:
            return "इनपुट/आउटपुट त्रुटि (I/O Error): " + detail;
        case ErrorType::WARNING:
            return "चेतावनी (Warning): " + detail;
        case ErrorType::INFO:
            return "सूचना (Info): " + detail;
        default:
            return "अज्ञात त्रुटि (Unknown Error): " + detail;
    }
}

std::string ErrorHandler::getEnglishMessage(ErrorType type, const std::string& detail) {
    switch (type) {
        case ErrorType::SYNTAX_ERROR:
            return "Syntax Error: " + detail;
        case ErrorType::LEXER_ERROR:
            return "Lexer Error: " + detail;
        case ErrorType::SEMANTIC_ERROR:
            return "Semantic Error: " + detail;
        case ErrorType::RUNTIME_ERROR:
            return "Runtime Error: " + detail;
        case ErrorType::TYPE_ERROR:
            return "Type Error: " + detail;
        case ErrorType::UNKNOWN_IDENTIFIER:
            return "Unknown Identifier: " + detail;
        case ErrorType::INVALID_OPERATION:
            return "Invalid Operation: " + detail;
        case ErrorType::FILE_NOT_FOUND:
            return "File Not Found: " + detail;
        case ErrorType::IO_ERROR:
            return "I/O Error: " + detail;
        case ErrorType::WARNING:
            return "Warning: " + detail;
        case ErrorType::INFO:
            return "Info: " + detail;
        default:
            return "Unknown Error: " + detail;
    }
}

void ErrorHandler::printColored(const std::string& text, const std::string& color) {
    if (verbosity_level > 0) {
        std::cout << color << text << COLOR_RESET << std::endl;
    }
}

void ErrorHandler::printError(ErrorType type, const std::string& message,
                              int line, int column) {
    if (verbosity_level == 0) return;
    
    incrementErrorCount();
    
    std::stringstream ss;
    ss << "\n" << COLOR_RED << "=== MANTRA Compiler Error ===" << COLOR_RESET << "\n";
    
    // Location info
    if (line > 0 && column > 0) {
        ss << COLOR_YELLOW << "Location: Line " << line << ", Column " << column 
           << COLOR_RESET << "\n";
    }
    
    // Hindi message
    ss << COLOR_RED << getHindiMessage(type, message) << COLOR_RESET << "\n";
    
    // English message (for clarity)
    if (verbosity_level > 1) {
        ss << COLOR_YELLOW << getEnglishMessage(type, message) << COLOR_RESET << "\n";
    }
    
    std::cerr << ss.str();
}

void ErrorHandler::printWarning(const std::string& message, int line, int column) {
    if (verbosity_level == 0) return;
    
    incrementWarningCount();
    
    std::stringstream ss;
    ss << "\n" << COLOR_YELLOW << "=== MANTRA Compiler Warning ===" << COLOR_RESET << "\n";
    
    // Location info
    if (line > 0 && column > 0) {
        ss << COLOR_YELLOW << "Location: Line " << line << ", Column " << column 
           << COLOR_RESET << "\n";
    }
    
    // Message
    ss << COLOR_YELLOW << "चेतावनी (Warning): " << message << COLOR_RESET << "\n";
    
    if (verbosity_level > 1) {
        ss << COLOR_YELLOW << "Warning: " << message << COLOR_RESET << "\n";
    }
    
    std::cerr << ss.str();
}

void ErrorHandler::throwError(ErrorType type, const std::string& message,
                              int line, int column, int exit_code) {
    printError(type, message, line, column);
    std::exit(exit_code);
}

void ErrorHandler::setVerbosity(int level) {
    verbosity_level = level;
}

int ErrorHandler::getVerbosity() {
    return verbosity_level;
}

// Additional helper functions for error reporting
std::string ErrorHandler::formatLocation(int line, int column) {
    if (line > 0 && column > 0) {
        return "Line " + std::to_string(line) + ", Column " + std::to_string(column);
    }
    return "Unknown Location";
}

void ErrorHandler::printErrorWithContext(ErrorType type, const std::string& message,
                                        const std::string& context, int line, int column) {
    if (verbosity_level == 0) return;
    
    std::stringstream ss;
    ss << "\n" << COLOR_RED << "=== MANTRA Compilation Error ===" << COLOR_RESET << "\n";
    ss << COLOR_YELLOW << formatLocation(line, column) << COLOR_RESET << "\n";
    ss << COLOR_RED << getHindiMessage(type, message) << COLOR_RESET << "\n";
    
    if (!context.empty()) {
        ss << COLOR_YELLOW << "Context: " << context << COLOR_RESET << "\n";
    }
    
    std::cerr << ss.str();
}

// Statistics tracking for errors
static int error_count = 0;
static int warning_count = 0;

int ErrorHandler::getErrorCount() {
    return error_count;
}

int ErrorHandler::getWarningCount() {
    return warning_count;
}

void ErrorHandler::resetCounts() {
    error_count = 0;
    warning_count = 0;
}

void ErrorHandler::incrementErrorCount() {
    error_count++;
}

void ErrorHandler::incrementWarningCount() {
    warning_count++;
}

// Statistics tracking and error reporting complete
// End of Error Handler implementation
} // namespace mantra
