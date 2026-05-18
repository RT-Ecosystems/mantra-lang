#include "../core/interpreter.h"
#include "../core/lexer.h"
#include "../core/parser.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using mantra::Lexer;
using mantra::Token;
using mantra::TokenType;

namespace {

void require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "टेस्ट विफल: " << message << std::endl;
        std::abort();
    }
}

void assertTokenType(const std::string& source, TokenType expected) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    require(!tokens.empty(), "टोकन खाली मिला");
    require(tokens.front().type == expected, "टोकन प्रकार मेल नहीं खाया");
}

void assertTokenLexeme(const std::string& source, TokenType expected, const std::string& lexeme) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    require(!tokens.empty(), "टोकन खाली मिला");
    require(tokens.front().type == expected, "टोकन प्रकार मेल नहीं खाया");
    require(tokens.front().lexeme == lexeme, "लेक्सेम मेल नहीं खाया");
}

void testKeywords() {
    std::vector<std::pair<std::string, TokenType>> keywords = {
        {"dikhao", TokenType::KW_DIKHAO},
        {"agar", TokenType::KW_AGAR},
        {"warna", TokenType::KW_WARNA},
        {"baarbaar", TokenType::KW_BAARBAAR},
        {"kaam", TokenType::KW_KAAM},
        {"wapas", TokenType::KW_WAPAS},
        {"sach", TokenType::KW_SACH},
        {"jhooth", TokenType::KW_JHOOTH},
        {"aur", TokenType::KW_AUR},
        {"ya", TokenType::KW_YA},
        {"nahi", TokenType::KW_NAHI},
        {"rakho", TokenType::KW_RAKHO},
        {"se", TokenType::KW_SE},
        {"tak", TokenType::KW_TAK},
        {"banao", TokenType::KW_BANAO},
        {"kaado", TokenType::KW_KAADO},
        {"endral", TokenType::KW_ENDRAL},
        {"illena", TokenType::KW_ILLENA},
        {"marumurai", TokenType::KW_MARUMURAI},
        {"seyal", TokenType::KW_SEYAL},
        {"thirumbu", TokenType::KW_THIRUMBU},
        {"unmai", TokenType::KW_UNMAI},
        {"dekhao", TokenType::KW_DEKHAO},
        {"batavo", TokenType::KW_BATAVO},
        {"dakhav", TokenType::KW_DAKHAV},
        {"chupinchu", TokenType::KW_CHUPINCHU},
        {"toro", TokenType::KW_TORO},
        {"engil", TokenType::KW_ENGIL},
        {"sathyam", TokenType::KW_SATHYAM},
        {"asathyam", TokenType::KW_ASATHYAM},
        {"jodi", TokenType::KW_JODI},
        {"nahole", TokenType::KW_NAHOLE},
        {"print", TokenType::KW_PRINT}
    };

    for (const auto& entry : keywords) {
        assertTokenType(entry.first, entry.second);
    }
}

void testStrings() {
    Lexer lexer("\"Namaste\" 'Duniya'\n\"Line1\\nLine2\"");
    auto tokens = lexer.tokenize();
    assert(tokens.size() >= 4);
    assert(tokens[0].type == TokenType::STRING);
    assert(tokens[0].value == "Namaste");
    assert(tokens[1].type == TokenType::STRING);
    assert(tokens[1].value == "Duniya");
    assert(tokens[3].type == TokenType::STRING);
    assert(tokens[3].value.find("Line1") != std::string::npos);
}

void testTripleQuotedStrings() {
    Lexer lexer("\"\"\"multi\nline\"\"\"");
    auto tokens = lexer.tokenize();
    assert(tokens.size() >= 2);
    assert(tokens[0].type == TokenType::STRING);
    assert(tokens[0].value.find("multi") != std::string::npos);
}

void testNumbers() {
    assertTokenType("123", TokenType::NUMBER_INT);
    assertTokenType("45.67", TokenType::NUMBER_FLOAT);
    assertTokenType("0x1F", TokenType::NUMBER_HEX);
    assertTokenLexeme("789", TokenType::NUMBER_INT, "789");
}

void testDelimiters() {
    Lexer lexer("() {} [] , ; : .");
    auto tokens = lexer.tokenize();
    std::vector<TokenType> expected = {
        TokenType::LPAREN,
        TokenType::RPAREN,
        TokenType::LBRACE,
        TokenType::RBRACE,
        TokenType::LBRACKET,
        TokenType::RBRACKET,
        TokenType::COMMA,
        TokenType::SEMICOLON,
        TokenType::COLON,
        TokenType::DOT
    };

    for (size_t i = 0; i < expected.size(); ++i) {
        assert(tokens[i].type == expected[i]);
    }
}

void testOperators() {
    Lexer lexer("+ - * / == != < <= > >= = && || !");
    auto tokens = lexer.tokenize();
    std::vector<TokenType> expected = {
        TokenType::OP_PLUS,
        TokenType::OP_MINUS,
        TokenType::OP_MULTIPLY,
        TokenType::OP_DIVIDE,
        TokenType::OP_EQUAL,
        TokenType::OP_NOT_EQUAL,
        TokenType::OP_LESS,
        TokenType::OP_LESS_EQUAL,
        TokenType::OP_GREATER,
        TokenType::OP_GREATER_EQUAL,
        TokenType::OP_ASSIGN,
        TokenType::OP_AND,
        TokenType::OP_OR,
        TokenType::OP_NOT
    };

    for (size_t i = 0; i < expected.size(); ++i) {
        assert(tokens[i].type == expected[i]);
    }
}

void testArrowsAndDots() {
    Lexer lexer("=> -> .");
    auto tokens = lexer.tokenize();
    assert(tokens[0].type == TokenType::ARROW);
    assert(tokens[1].type == TokenType::DOUBLE_ARROW);
    assert(tokens[2].type == TokenType::DOT);
}

void testIdentifiers() {
    Lexer lexer("variable_name anotherVar");
    auto tokens = lexer.tokenize();
    assert(tokens[0].type == TokenType::IDENTIFIER);
    assert(tokens[0].lexeme == "variable_name");
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[1].lexeme == "anotherVar");
}

void testCommentsAndNewlines() {
    Lexer lexer("dikhao 1 # comment\nprint 2");
    auto tokens = lexer.tokenize();
    assert(tokens.size() >= 5);
    assert(tokens[0].type == TokenType::KW_DIKHAO);
    assert(tokens[1].type == TokenType::NUMBER_INT);
    assert(tokens[2].type == TokenType::NEWLINE);
    assert(tokens[3].type == TokenType::KW_PRINT);
    assert(tokens[4].type == TokenType::NUMBER_INT);
}

void testWindowsNewline() {
    Lexer lexer("print 1\r\nprint 2");
    auto tokens = lexer.tokenize();
    assert(tokens[0].type == TokenType::KW_PRINT);
    assert(tokens[2].type == TokenType::NEWLINE);
    assert(tokens[3].type == TokenType::KW_PRINT);
}

void testErrors() {
    Lexer lexer("@ ???");
    auto tokens = lexer.tokenize();
    assert(tokens.front().type == TokenType::UNKNOWN);
    assert(tokens[1].type == TokenType::UNKNOWN);
}

void testBaarBaarForLoopParsing() {
    const std::string source = "baarbaar i = 1 se 5 tak\n  dikhao i\n";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    mantra::Parser parser(tokens);
    auto program = parser.parseProgram();
    require(!parser.hasError(), "Loop example failed to parse");
    require(program != nullptr, "Program node not found");

    std::ostringstream output;
    auto* previous = std::cout.rdbuf(output.rdbuf());
    try {
        mantra::Interpreter interpreter;
        interpreter.interpret(*program);
    } catch (...) {
        std::cout.rdbuf(previous);
        throw;
    }
    std::cout.rdbuf(previous);

    require(output.str() == "1\n2\n3\n4\n5\n", "for-loop output is incorrect");
}

} // namespace

int main() {
    testKeywords();
    testStrings();
    testTripleQuotedStrings();
    testNumbers();
    testDelimiters();
    testOperators();
    testArrowsAndDots();
    testIdentifiers();
    testCommentsAndNewlines();
    testWindowsNewline();
    testErrors();
    testBaarBaarForLoopParsing();

    std::cout << "सभी लेक्सर टेस्ट सफल रहे।" << std::endl;
    return 0;
}
