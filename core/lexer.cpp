#include "lexer.h"
#include <iostream>
#include <cctype>
#include <stdexcept>

namespace mantra {

// Initialize keyword map with all 22 language mixes + English
const std::unordered_map<std::string, TokenType> Lexer::keyword_map = {
    // HINGLISH (Hindi + English)
    {"dikhao", TokenType::KW_DIKHAO},
    {"agar", TokenType::KW_AGAR},
    {"tab", TokenType::KW_TAB},
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

    // TANGLISH (Tamil + English)
    {"kaado", TokenType::KW_KAADO},
    {"endral", TokenType::KW_ENDRAL},
    {"illena", TokenType::KW_ILLENA},
    {"marumurai", TokenType::KW_MARUMURAI},
    {"seyal", TokenType::KW_SEYAL},
    {"thirumbu", TokenType::KW_THIRUMBU},
    {"unmai", TokenType::KW_UNMAI},
    {"poi", TokenType::KW_POI},
    {"mattrum", TokenType::KW_MATTRUM},
    {"allathu", TokenType::KW_ALLATHU},

    // PUNGLISH (Punjabi + English)
    {"dakho", TokenType::KW_DAKHO},
    {"jekr", TokenType::KW_JEKR},
    {"mudmudke", TokenType::KW_MUDMUDKE},
    {"murhke", TokenType::KW_MURHKE},
    {"te", TokenType::KW_TE},
    {"ja", TokenType::KW_JA},

    // BENGLISH (Bengali + English)
    {"dekhao", TokenType::KW_DEKHAO},
    {"jodi", TokenType::KW_JODI},
    {"nahole", TokenType::KW_NAHOLE},
    {"jabotokkhon", TokenType::KW_JABOTOKKHON},
    {"kaj", TokenType::KW_KAJ},
    {"phrao", TokenType::KW_PHRAO},
    {"satya", TokenType::KW_SATYA},
    {"mittha", TokenType::KW_MITTHA},
    {"ebong", TokenType::KW_EBONG},

    // GUJLISH (Gujarati + English)
    {"batavo", TokenType::KW_BATAVO},
    {"jor", TokenType::KW_JOR},
    {"nahi_to", TokenType::KW_NAHI_TO},
    {"jabtak", TokenType::KW_JABTAK},
    {"pachhu", TokenType::KW_PACHHU},
    {"sachu", TokenType::KW_SACHU},
    {"juthu", TokenType::KW_JUTHU},
    {"ane", TokenType::KW_ANE},
    {"ke", TokenType::KW_KE},

    // MARATHLISH (Marathi + English)
    {"dakhav", TokenType::KW_DAKHAV},
    {"jar", TokenType::KW_JAR},
    {"nahitar", TokenType::KW_NAHITAR},
    {"parat", TokenType::KW_PARAT},
    {"kharay", TokenType::KW_KHARAY},
    {"khota", TokenType::KW_KHOTA},
    {"ani", TokenType::KW_ANI},
    {"kiva", TokenType::KW_KIVA},

    // TELGLISH (Telugu + English)
    {"chupinchu", TokenType::KW_CHUPINCHU},
    {"ante", TokenType::KW_ANTE},
    {"lekapote", TokenType::KW_LEKAPOTE},
    {"andaka", TokenType::KW_ANDAKA},
    {"pani", TokenType::KW_PANI},
    {"tirigi", TokenType::KW_TIRIGI},
    {"nijam", TokenType::KW_NIJAM},
    {"abaddham", TokenType::KW_ABADDHAM},
    {"mariyu", TokenType::KW_MARIYU},
    {"leda", TokenType::KW_LEDA},

    // KANGLISH (Kannada + English)
    {"toro", TokenType::KW_TORO},
    {"agre", TokenType::KW_AGRE},
    {"illandre", TokenType::KW_ILLANDRE},
    {"jotagje", TokenType::KW_JOTAGJE},
    {"kelu", TokenType::KW_KELU},
    {"hogi", TokenType::KW_HOGI},
    {"nijha", TokenType::KW_NIJHA},
    {"sullu", TokenType::KW_SULLU},
    {"mattu", TokenType::KW_MATTU},
    {"athava", TokenType::KW_ATHAVA},

    // MANGLISH (Malayalam + English)
    {"kaaniku", TokenType::KW_KAANIKU},
    {"engil", TokenType::KW_ENGIL},
    {"allenkil", TokenType::KW_ALLENKIL},
    {"varethum", TokenType::KW_VARETHUM},
    {"cheyu", TokenType::KW_CHEYU},
    {"matti", TokenType::KW_MATTI},
    {"sathyam", TokenType::KW_SATHYAM},
    {"asathyam", TokenType::KW_ASATHYAM},
    {"um", TokenType::KW_UM},

    // ODLISH (Oriya + English)
    {"jadi", TokenType::KW_JADI},
    {"jabat", TokenType::KW_JABAT},
    {"phera", TokenType::KW_PHERA},
    {"sata", TokenType::KW_SATA},
    {"mithya", TokenType::KW_MITHYA},
    {"ebam", TokenType::KW_EBAM},

    // ASSLISH (Assamese + English)
    {"nohole", TokenType::KW_NOHOLE},
    {"jabotloi", TokenType::KW_JABOTLOI},
    {"ghurai", TokenType::KW_GHURAI},
    {"mitha", TokenType::KW_MITHA},
    {"aru", TokenType::KW_ARU},

    // MAITHLISH (Maithili + English)
    {"dekhaow", TokenType::KW_DEKHAOW},
    {"je", TokenType::KW_JE},
    {"nahi_ta", TokenType::KW_NAHI_TA},

    // KONKLISH (Konkani + English)
    {"dakhoi", TokenType::KW_DAKHOI},
    {"zor", TokenType::KW_ZOR},
    {"naitar", TokenType::KW_NAITAR},
    {"parot", TokenType::KW_PAROT},
    {"va", TokenType::KW_VA},

    // NEPLISH (Nepali + English)
    {"dekhau", TokenType::KW_DEKHAU},
    {"yedi", TokenType::KW_YEDI},
    {"nabhaye", TokenType::KW_NABHAYE},
    {"jabsamma", TokenType::KW_JABSAMMA},
    {"pharka", TokenType::KW_PHARKA},
    {"sano", TokenType::KW_SANO},
    {"jhuto", TokenType::KW_JHUTO},
    {"ra", TokenType::KW_RA},
    {"wa", TokenType::KW_WA},

    // SANGLISH (Sanskrit + English)
    {"darshaya", TokenType::KW_DARSHAYA},
    {"yadi", TokenType::KW_YADI},
    {"anyatha", TokenType::KW_ANYATHA},
    {"yavat", TokenType::KW_YAVAT},
    {"karya", TokenType::KW_KARYA},
    {"pratyaga", TokenType::KW_PRATYAGA},
    {"cha", TokenType::KW_CHA},

    // KASHLISH (Kashmiri + English)
    {"waatav", TokenType::KW_WAATAV},
    {"galaw", TokenType::KW_GALAW},

    // SINDLISH (Sindhi + English)
    {"dikhay", TokenType::KW_DIKHAY},
    {"khoota", TokenType::KW_KHOOTA},
    {"ain", TokenType::KW_AIN},

    // DOGLISH (Dogri + English)
    {"wekho", TokenType::KW_WEKHO},
    {"ger", TokenType::KW_GER},
    {"nahin_ta", TokenType::KW_NAHIN_TA},

    // MANLISH (Manipuri + English)
    {"nungsi", TokenType::KW_NUNGSI},
    {"amare", TokenType::KW_AMARE},
    {"natte", TokenType::KW_NATTE},
    {"chatlaba", TokenType::KW_CHATLABA},
    {"thabak", TokenType::KW_THABAK},
    {"chingba", TokenType::KW_CHINGBA},
    {"thambal", TokenType::KW_THAMBAL},
    {"makhal", TokenType::KW_MAKHAL},
    {"amasung", TokenType::KW_AMASUNG},
    {"yanaba", TokenType::KW_YANABA},

    // BODLISH (Bodo + English)
    {"nangi", TokenType::KW_NANGI},
    {"bodo", TokenType::KW_BODO},
    {"nangou", TokenType::KW_NANGOU},
    {"jabsim", TokenType::KW_JABSIM},
    {"phirave", TokenType::KW_PHIRAVE},
    {"sato", TokenType::KW_SATO},
    {"beo", TokenType::KW_BEO},
    {"ar", TokenType::KW_AR},
    {"abua", TokenType::KW_ABUA},

    // SANTLISH (Santhali + English)
    {"dado", TokenType::KW_DADO},
    {"ama", TokenType::KW_AMA},
    {"bae", TokenType::KW_BAE},
    {"jabto", TokenType::KW_JABTO},
    {"tikin", TokenType::KW_TIKIN},
    {"hon", TokenType::KW_HON},
    {"bae_hon", TokenType::KW_BAE_HON},
    {"noa", TokenType::KW_NOA},

    // English keywords
    {"print", TokenType::KW_PRINT},
    {"if", TokenType::KW_IF},
    {"else", TokenType::KW_ELSE},
    {"while", TokenType::KW_WHILE},
    {"for", TokenType::KW_FOR},
    {"function", TokenType::KW_FUNCTION},
    {"return", TokenType::KW_RETURN},
    {"true", TokenType::KW_TRUE},
    {"false", TokenType::KW_FALSE},
    {"and", TokenType::KW_AND},
    {"or", TokenType::KW_OR},
    {"not", TokenType::KW_NOT},
    {"let", TokenType::KW_LET},
    {"from", TokenType::KW_FROM},
    {"to", TokenType::KW_TO},
    {"then", TokenType::KW_THEN},
    {"create", TokenType::KW_CREATE},
    {"class", TokenType::KW_CLASS},
    {"import", TokenType::KW_IMPORT},
};

Lexer::Lexer(const std::string& src) 
    : source(src), position(0), line(1), column(1), line_start(0) {}

char Lexer::currentChar() {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char Lexer::peekChar(int offset) {
    size_t peek_pos = position + offset;
    if (peek_pos >= source.length()) {
        return '\0';
    }
    return source[peek_pos];
}

char Lexer::nextChar() {
    if (position >= source.length()) {
        return '\0';
    }
    char ch = source[position++];
    if (ch == '\n') {
        line++;
        column = 1;
        line_start = position;
    } else {
        column++;
    }
    return ch;
}

bool Lexer::isWhitespace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

bool Lexer::isNewline(char ch) {
    return ch == '\n' || ch == '\r';
}

bool Lexer::isDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

bool Lexer::isHexDigit(char ch) {
    return isDigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

bool Lexer::isAlpha(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

bool Lexer::isAlphaNumeric(char ch) {
    return isAlpha(ch) || isDigit(ch);
}

bool Lexer::isOperator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' ||
           ch == '=' || ch == '<' || ch == '>' || ch == '!' || ch == '&' || ch == '|';
}

void Lexer::skipWhitespace() {
    while (isWhitespace(currentChar()) && currentChar() != '\n') {
        nextChar();
    }
}

void Lexer::skipComment() {
    if (currentChar() == '#') {
        while (currentChar() != '\n' && currentChar() != '\0') {
            nextChar();
        }
    }
}

Token Lexer::readString(char quote) {
    int start_line = line;
    int start_col = column;
    
    nextChar(); // Skip opening quote
    std::string value;
    
    while (currentChar() != quote && currentChar() != '\0') {
        if (currentChar() == '\\') {
            nextChar();
            switch (currentChar()) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                case '0': value += '\0'; break;
                default: value += currentChar();
            }
            nextChar();
        } else {
            value += currentChar();
            nextChar();
        }
    }
    
    if (currentChar() == '\0') {
        reportError("Unterminated string at line " + std::to_string(start_line) + 
                   ", column " + std::to_string(start_col));
        return Token(TokenType::ERROR_TOKEN, "", start_line, start_col);
    }
    
    nextChar(); // Skip closing quote
    
    Token token(TokenType::STRING, value, start_line, start_col);
    token.value = value;
    return token;
}

Token Lexer::readTripleQuoteString(char quote) {
    int start_line = line;
    int start_col = column;
    
    nextChar(); nextChar(); nextChar(); // Skip opening triple quotes
    std::string value;
    
    while ((currentChar() != quote || peekChar() != quote || peekChar(2) != quote) 
           && currentChar() != '\0') {
        value += currentChar();
        nextChar();
    }
    
    if (currentChar() == '\0') {
        reportError("Unterminated triple-quoted string at line " + std::to_string(start_line));
        return Token(TokenType::ERROR_TOKEN, "", start_line, start_col);
    }
    
    nextChar(); nextChar(); nextChar(); // Skip closing triple quotes
    
    Token token(TokenType::STRING, value, start_line, start_col);
    token.value = value;
    return token;
}

Token Lexer::readNumber() {
    int start_line = line;
    int start_col = column;
    std::string value;
    
    // Check for hex number
    if (currentChar() == '0' && (peekChar() == 'x' || peekChar() == 'X')) {
        value += currentChar();
        nextChar();
        value += currentChar();
        nextChar();
        
        while (isHexDigit(currentChar())) {
            value += currentChar();
            nextChar();
        }
        return Token(TokenType::NUMBER_HEX, value, start_line, start_col);
    }
    
    // Read integer part
    while (isDigit(currentChar())) {
        value += currentChar();
        nextChar();
    }
    
    // Check for float
    if (currentChar() == '.' && isDigit(peekChar())) {
        value += currentChar();
        nextChar();
        
        while (isDigit(currentChar())) {
            value += currentChar();
            nextChar();
        }
        
        // Check for scientific notation
        if (currentChar() == 'e' || currentChar() == 'E') {
            value += currentChar();
            nextChar();
            
            if (currentChar() == '+' || currentChar() == '-') {
                value += currentChar();
                nextChar();
            }
            
            while (isDigit(currentChar())) {
                value += currentChar();
                nextChar();
            }
        }
        
        return Token(TokenType::NUMBER_FLOAT, value, start_line, start_col);
    }
    
    return Token(TokenType::NUMBER_INT, value, start_line, start_col);
}

Token Lexer::readIdentifierOrKeyword() {
    int start_line = line;
    int start_col = column;
    std::string value;
    
    while (isAlphaNumeric(currentChar())) {
        value += currentChar();
        nextChar();
    }
    
    // Convert to lowercase for keyword lookup
    std::string lower_value = value;
    for (char& c : lower_value) {
        c = std::tolower(c);
    }
    
    // Check if it's a keyword
    auto it = keyword_map.find(lower_value);
    if (it != keyword_map.end()) {
        return Token(it->second, value, start_line, start_col);
    }
    
    return Token(TokenType::IDENTIFIER, value, start_line, start_col);
}

void Lexer::reportError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    // Skip comments
    while (currentChar() == '#') {
        skipComment();
        skipWhitespace();
    }
    
    int start_line = line;
    int start_col = column;
    
    // End of file
    if (currentChar() == '\0') {
        return Token(TokenType::EOF_TOKEN, "", start_line, start_col);
    }
    
    // Newline
    if (currentChar() == '\n') {
        nextChar();
        return Token(TokenType::NEWLINE, "\n", start_line, start_col);
    }
    
    // Handle carriage return + newline (Windows line endings)
    if (currentChar() == '\r') {
        nextChar();
        if (currentChar() == '\n') {
            nextChar();
        }
        return Token(TokenType::NEWLINE, "\r\n", start_line, start_col);
    }
    
    // String literals
    if (currentChar() == '"' && peekChar() == '"' && peekChar(2) == '"') {
        return readTripleQuoteString('"');
    }
    if (currentChar() == '\'' && peekChar() == '\'' && peekChar(2) == '\'') {
        return readTripleQuoteString('\'');
    }
    if (currentChar() == '"') {
        return readString('"');
    }
    if (currentChar() == '\'') {
        return readString('\'');
    }
    
    // Numbers
    if (isDigit(currentChar())) {
        return readNumber();
    }
    
    // Identifiers and keywords
    if (isAlpha(currentChar())) {
        return readIdentifierOrKeyword();
    }
    
    // Operators and delimiters
    char ch = currentChar();
    
    switch (ch) {
        case '+': nextChar(); return Token(TokenType::OP_PLUS, "+", start_line, start_col);
        case '%': nextChar(); return Token(TokenType::OP_MODULO, "%", start_line, start_col);
        case '(': nextChar(); return Token(TokenType::LPAREN, "(", start_line, start_col);
        case ')': nextChar(); return Token(TokenType::RPAREN, ")", start_line, start_col);
        case '{': nextChar(); return Token(TokenType::LBRACE, "{", start_line, start_col);
        case '}': nextChar(); return Token(TokenType::RBRACE, "}", start_line, start_col);
        case '[': nextChar(); return Token(TokenType::LBRACKET, "[", start_line, start_col);
        case ']': nextChar(); return Token(TokenType::RBRACKET, "]", start_line, start_col);
        case ';': nextChar(); return Token(TokenType::SEMICOLON, ";", start_line, start_col);
        case ',': nextChar(); return Token(TokenType::COMMA, ",", start_line, start_col);
        case ':': nextChar(); return Token(TokenType::COLON, ":", start_line, start_col);
        
        case '-':
            nextChar();
            if (currentChar() == '>') {
                nextChar();
                return Token(TokenType::DOUBLE_ARROW, "->", start_line, start_col);
            }
            return Token(TokenType::OP_MINUS, "-", start_line, start_col);
        
        case '*':
            nextChar();
            return Token(TokenType::OP_MULTIPLY, "*", start_line, start_col);
        
        case '/':
            nextChar();
            return Token(TokenType::OP_DIVIDE, "/", start_line, start_col);
        
        case '=':
            nextChar();
            if (currentChar() == '=') {
                nextChar();
                return Token(TokenType::OP_EQUAL, "==", start_line, start_col);
            } else if (currentChar() == '>') {
                nextChar();
                return Token(TokenType::ARROW, "=>", start_line, start_col);
            }
            return Token(TokenType::OP_ASSIGN, "=", start_line, start_col);
        
        case '<':
            nextChar();
            if (currentChar() == '=') {
                nextChar();
                return Token(TokenType::OP_LESS_EQUAL, "<=", start_line, start_col);
            }
            return Token(TokenType::OP_LESS, "<", start_line, start_col);
        
        case '>':
            nextChar();
            if (currentChar() == '=') {
                nextChar();
                return Token(TokenType::OP_GREATER_EQUAL, ">=", start_line, start_col);
            }
            return Token(TokenType::OP_GREATER, ">", start_line, start_col);
        
        case '!':
            nextChar();
            if (currentChar() == '=') {
                nextChar();
                return Token(TokenType::OP_NOT_EQUAL, "!=", start_line, start_col);
            }
            return Token(TokenType::OP_NOT, "!", start_line, start_col);
        
        case '&':
            nextChar();
            if (currentChar() == '&') {
                nextChar();
                return Token(TokenType::OP_AND, "&&", start_line, start_col);
            }
            break;
        
        case '|':
            nextChar();
            if (currentChar() == '|') {
                nextChar();
                return Token(TokenType::OP_OR, "||", start_line, start_col);
            }
            break;
        
        case '.':
            nextChar();
            return Token(TokenType::DOT, ".", start_line, start_col);
    }
    
    reportError("Unexpected character '" + std::string(1, ch) + "' at line " + 
               std::to_string(start_line) + ", column " + std::to_string(start_col));
    nextChar();
    return Token(TokenType::UNKNOWN, std::string(1, ch), start_line, start_col);
}

std::vector<Token> Lexer::tokenize() {
    Token tok = nextToken();
    while (tok.type != TokenType::EOF_TOKEN) {
        tokens.push_back(tok);
        tok = nextToken();
    }
    tokens.push_back(tok); // Add EOF token
    return tokens;
}

Token Lexer::peekToken() {
    if (position < tokens.size()) {
        return tokens[position];
    }
    return Token(TokenType::EOF_TOKEN, "", line, column);
}

void Lexer::printTokens() const {
    for (const auto& token : tokens) {
        std::cout << "Token: " << tokenTypeToString(token.type) 
                  << " | Lexeme: " << token.lexeme 
                  << " | Line: " << token.line 
                  << " | Column: " << token.column << std::endl;
    }
}

} // namespace mantra
