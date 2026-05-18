#ifndef MANTRA_TOKEN_H
#define MANTRA_TOKEN_H

#include <string>
#include <unordered_map>
#include <memory>

namespace mantra {

// TokenType enum containing all token types for MANTRA language
// Supports all 22 Indian language mixes plus English
enum class TokenType {
    // Literals
    NUMBER_INT,
    NUMBER_FLOAT,
    NUMBER_HEX,
    STRING,
    IDENTIFIER,

    // Keywords - HINGLISH (Hindi + English)
    KW_DIKHAO,      // print
    KW_AGAR,        // if
    KW_TAB,         // then
    KW_WARNA,       // else
    KW_BAARBAAR,    // while
    KW_KAAM,        // function
    KW_WAPAS,       // return
    KW_SACH,        // true
    KW_JHOOTH,      // false
    KW_AUR,         // and
    KW_YA,          // or
    KW_NAHI,        // not
    KW_RAKHO,       // let
    KW_SE,          // from
    KW_TAK,         // to
    KW_BANAO,       // create

    // Keywords - TANGLISH (Tamil + English)
    KW_KAADO,       // print
    KW_ENDRAL,      // if
    KW_ILLENA,      // else
    KW_MARUMURAI,   // while
    KW_SEYAL,       // function
    KW_THIRUMBU,    // return
    KW_UNMAI,       // true
    KW_POI,         // false
    KW_MATTRUM,     // and
    KW_ALLATHU,     // or

    // Keywords - PUNGLISH (Punjabi + English)
    KW_DAKHO,       // print
    KW_JEKR,        // if
    KW_MUDMUDKE,    // else
    KW_MURHKE,      // while
    KW_TE,          // function
    KW_JA,          // return

    // Keywords - BENGLISH (Bengali + English)
    KW_DEKHAO,      // print
    KW_JODI,        // if
    KW_NAHOLE,      // else
    KW_JABOTOKKHON, // while
    KW_KAJ,         // function
    KW_PHRAO,       // return
    KW_SATYA,       // true
    KW_MITTHA,      // false
    KW_EBONG,       // and

    // Keywords - GUJLISH (Gujarati + English)
    KW_BATAVO,      // print
    KW_JOR,         // if
    KW_NAHI_TO,     // else
    KW_JABTAK,      // while
    KW_PACHHU,      // function
    KW_SACHU,       // true
    KW_JUTHU,       // false
    KW_ANE,         // and
    KW_KE,          // or

    // Keywords - MARATHLISH (Marathi + English)
    KW_DAKHAV,      // print
    KW_JAR,         // if
    KW_NAHITAR,     // else
    KW_PARAT,       // while
    KW_KHARAY,      // function
    KW_KHOTA,       // true
    KW_ANI,         // and
    KW_KIVA,        // or

    // Keywords - TELGLISH (Telugu + English)
    KW_CHUPINCHU,   // print
    KW_ANTE,        // if
    KW_LEKAPOTE,    // else
    KW_ANDAKA,      // while
    KW_PANI,        // function
    KW_TIRIGI,      // return
    KW_NIJAM,       // true
    KW_ABADDHAM,    // false
    KW_MARIYU,      // and
    KW_LEDA,        // or

    // Keywords - KANGLISH (Kannada + English)
    KW_TORO,        // print
    KW_AGRE,        // if
    KW_ILLANDRE,    // else
    KW_JOTAGJE,     // while
    KW_KELU,        // function
    KW_HOGI,        // return
    KW_NIJHA,       // true
    KW_SULLU,       // false
    KW_MATTU,       // and
    KW_ATHAVA,      // or

    // Keywords - MANGLISH (Malayalam + English)
    KW_KAANIKU,     // print
    KW_ENGIL,       // if
    KW_ALLENKIL,    // else
    KW_VARETHUM,    // while
    KW_CHEYU,       // function
    KW_MATTI,       // return
    KW_SATHYAM,     // true
    KW_ASATHYAM,    // false
    KW_UM,          // and

    // Keywords - ODLISH (Oriya + English)
    KW_JADI,        // if
    KW_JABAT,       // while
    KW_PHERA,       // return
    KW_SATA,        // true
    KW_MITHYA,      // false
    KW_EBAM,        // and

    // Keywords - ASSLISH (Assamese + English)
    KW_NOHOLE,      // if
    KW_JABOTLOI,    // while
    KW_GHURAI,      // return
    KW_MITHA,       // false
    KW_ARU,         // and

    // Keywords - MAITHLISH (Maithili + English)
    KW_DEKHAOW,     // print
    KW_JE,          // if
    KW_NAHI_TA,     // else

    // Keywords - KONKLISH (Konkani + English)
    KW_DAKHOI,      // print
    KW_ZOR,         // if
    KW_NAITAR,      // else
    KW_PAROT,       // return
    KW_VA,          // and

    // Keywords - NEPLISH (Nepali + English)
    KW_DEKHAU,      // print
    KW_YEDI,        // if
    KW_NABHAYE,     // else
    KW_JABSAMMA,    // while
    KW_PHARKA,      // return
    KW_SANO,        // true
    KW_JHUTO,       // false
    KW_RA,          // and
    KW_WA,          // or

    // Keywords - SANGLISH (Sanskrit + English)
    KW_DARSHAYA,    // print
    KW_YADI,        // if
    KW_ANYATHA,     // else
    KW_YAVAT,       // while
    KW_KARYA,       // function
    KW_PRATYAGA,    // return
    KW_CHA,         // and

    // Keywords - KASHLISH (Kashmiri + English)
    KW_WAATAV,      // print
    KW_GALAW,       // if

    // Keywords - SINDLISH (Sindhi + English)
    KW_DIKHAY,      // print
    KW_KHOOTA,      // false
    KW_AIN,         // and

    // Keywords - DOGLISH (Dogri + English)
    KW_WEKHO,       // print
    KW_GER,         // if
    KW_NAHIN_TA,    // else

    // Keywords - MANLISH (Manipuri + English)
    KW_NUNGSI,      // print
    KW_AMARE,       // if
    KW_NATTE,       // else
    KW_CHATLABA,    // while
    KW_THABAK,      // function
    KW_CHINGBA,     // return
    KW_THAMBAL,     // true
    KW_MAKHAL,      // false
    KW_AMASUNG,     // and
    KW_YANABA,      // or

    // Keywords - BODLISH (Bodo + English)
    KW_NANGI,       // print
    KW_BODO,        // if
    KW_NANGOU,      // else
    KW_JABSIM,      // while
    KW_PHIRAVE,     // function
    KW_SATO,        // return
    KW_BEO,         // true
    KW_AR,          // and
    KW_ABUA,        // or

    // Keywords - SANTLISH (Santhali + English)
    KW_DADO,        // print
    KW_AMA,         // if
    KW_BAE,         // else
    KW_JABTO,       // while
    KW_TIKIN,       // function
    KW_HON,         // return
    KW_BAE_HON,     // true
    KW_NOA,         // false

    // English keywords
    KW_PRINT,       // print
    KW_IF,          // if
    KW_ELSE,        // else
    KW_WHILE,       // while
    KW_FOR,         // for
    KW_FUNCTION,    // function
    KW_RETURN,      // return
    KW_TRUE,        // true
    KW_FALSE,       // false
    KW_AND,         // and
    KW_OR,          // or
    KW_NOT,         // not
    KW_LET,         // let
    KW_FROM,        // from
    KW_TO,          // to
    KW_THEN,        // then
    KW_CREATE,      // create
    KW_CLASS,       // class
    KW_IMPORT,      // import
    KW_BREAK,       // break
    KW_CONTINUE,    // continue

    // Operators
    OP_PLUS,        // +
    OP_MINUS,       // -
    OP_MULTIPLY,    // *
    OP_DIVIDE,      // /
    OP_MODULO,      // %
    OP_ASSIGN,      // =
    OP_EQUAL,       // ==
    OP_NOT_EQUAL,   // !=
    OP_LESS,        // <
    OP_LESS_EQUAL,  // <=
    OP_GREATER,     // >
    OP_GREATER_EQUAL, // >=
    OP_AND,         // &&
    OP_OR,          // ||
    OP_NOT,         // !

    // Delimiters
    LPAREN,         // (
    RPAREN,         // )
    LBRACE,         // {
    RBRACE,         // }
    LBRACKET,       // [
    RBRACKET,       // ]
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    COLON,          // :
    ARROW,          // =>
    DOUBLE_ARROW,   // ->

    // Special
    NEWLINE,
    EOF_TOKEN,
    UNKNOWN,
    ERROR_TOKEN
};

// Token structure containing token information
struct Token {
    TokenType type;
    std::string lexeme;
    std::string value;
    int line;
    int column;
    int start_pos;   // byte position in source
    int end_pos;     // byte position in source

    Token() : type(TokenType::UNKNOWN), line(0), column(0), start_pos(0), end_pos(0) {}
    
    Token(TokenType t, const std::string& lex, int l, int c) 
        : type(t), lexeme(lex), line(l), column(c), start_pos(0), end_pos(0) {}
};

// Helper function to convert TokenType to string
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        // Literals
        case TokenType::NUMBER_INT: return "NUMBER_INT";
        case TokenType::NUMBER_FLOAT: return "NUMBER_FLOAT";
        case TokenType::NUMBER_HEX: return "NUMBER_HEX";
        case TokenType::STRING: return "STRING";
        case TokenType::IDENTIFIER: return "IDENTIFIER";

        // HINGLISH keywords
        case TokenType::KW_DIKHAO: return "DIKHAO";
        case TokenType::KW_AGAR: return "AGAR";
        case TokenType::KW_TAB: return "TAB";
        case TokenType::KW_WARNA: return "WARNA";
        case TokenType::KW_BAARBAAR: return "BAARBAAR";
        case TokenType::KW_KAAM: return "KAAM";
        case TokenType::KW_WAPAS: return "WAPAS";
        case TokenType::KW_SACH: return "SACH";
        case TokenType::KW_JHOOTH: return "JHOOTH";
        case TokenType::KW_AUR: return "AUR";
        case TokenType::KW_YA: return "YA";
        case TokenType::KW_NAHI: return "NAHI";
        case TokenType::KW_RAKHO: return "RAKHO";
        case TokenType::KW_SE: return "SE";
        case TokenType::KW_TAK: return "TAK";
        case TokenType::KW_BANAO: return "BANAO";

        // TANGLISH keywords
        case TokenType::KW_KAADO: return "KAADO";
        case TokenType::KW_ENDRAL: return "ENDRAL";
        case TokenType::KW_ILLENA: return "ILLENA";
        case TokenType::KW_MARUMURAI: return "MARUMURAI";
        case TokenType::KW_SEYAL: return "SEYAL";
        case TokenType::KW_THIRUMBU: return "THIRUMBU";
        case TokenType::KW_UNMAI: return "UNMAI";
        case TokenType::KW_POI: return "POI";
        case TokenType::KW_MATTRUM: return "MATTRUM";
        case TokenType::KW_ALLATHU: return "ALLATHU";

        // PUNGLISH keywords
        case TokenType::KW_DAKHO: return "DAKHO";
        case TokenType::KW_JEKR: return "JEKR";
        case TokenType::KW_MUDMUDKE: return "MUDMUDKE";
        case TokenType::KW_MURHKE: return "MURHKE";
        case TokenType::KW_TE: return "TE";
        case TokenType::KW_JA: return "JA";

        // BENGLISH keywords
        case TokenType::KW_DEKHAO: return "DEKHAO";
        case TokenType::KW_JODI: return "JODI";
        case TokenType::KW_NAHOLE: return "NAHOLE";
        case TokenType::KW_JABOTOKKHON: return "JABOTOKKHON";
        case TokenType::KW_KAJ: return "KAJ";
        case TokenType::KW_PHRAO: return "PHRAO";
        case TokenType::KW_SATYA: return "SATYA";
        case TokenType::KW_MITTHA: return "MITTHA";
        case TokenType::KW_EBONG: return "EBONG";

        // GUJLISH keywords
        case TokenType::KW_BATAVO: return "BATAVO";
        case TokenType::KW_JOR: return "JOR";
        case TokenType::KW_NAHI_TO: return "NAHI_TO";
        case TokenType::KW_JABTAK: return "JABTAK";
        case TokenType::KW_PACHHU: return "PACHHU";
        case TokenType::KW_SACHU: return "SACHU";
        case TokenType::KW_JUTHU: return "JUTHU";
        case TokenType::KW_ANE: return "ANE";
        case TokenType::KW_KE: return "KE";

        // MARATHLISH keywords
        case TokenType::KW_DAKHAV: return "DAKHAV";
        case TokenType::KW_JAR: return "JAR";
        case TokenType::KW_NAHITAR: return "NAHITAR";
        case TokenType::KW_PARAT: return "PARAT";
        case TokenType::KW_KHARAY: return "KHARAY";
        case TokenType::KW_KHOTA: return "KHOTA";
        case TokenType::KW_ANI: return "ANI";
        case TokenType::KW_KIVA: return "KIVA";

        // TELGLISH keywords
        case TokenType::KW_CHUPINCHU: return "CHUPINCHU";
        case TokenType::KW_ANTE: return "ANTE";
        case TokenType::KW_LEKAPOTE: return "LEKAPOTE";
        case TokenType::KW_ANDAKA: return "ANDAKA";
        case TokenType::KW_PANI: return "PANI";
        case TokenType::KW_TIRIGI: return "TIRIGI";
        case TokenType::KW_NIJAM: return "NIJAM";
        case TokenType::KW_ABADDHAM: return "ABADDHAM";
        case TokenType::KW_MARIYU: return "MARIYU";
        case TokenType::KW_LEDA: return "LEDA";

        // KANGLISH keywords
        case TokenType::KW_TORO: return "TORO";
        case TokenType::KW_AGRE: return "AGRE";
        case TokenType::KW_ILLANDRE: return "ILLANDRE";
        case TokenType::KW_JOTAGJE: return "JOTAGJE";
        case TokenType::KW_KELU: return "KELU";
        case TokenType::KW_HOGI: return "HOGI";
        case TokenType::KW_NIJHA: return "NIJHA";
        case TokenType::KW_SULLU: return "SULLU";
        case TokenType::KW_MATTU: return "MATTU";
        case TokenType::KW_ATHAVA: return "ATHAVA";

        // MANGLISH keywords
        case TokenType::KW_KAANIKU: return "KAANIKU";
        case TokenType::KW_ENGIL: return "ENGIL";
        case TokenType::KW_ALLENKIL: return "ALLENKIL";
        case TokenType::KW_VARETHUM: return "VARETHUM";
        case TokenType::KW_CHEYU: return "CHEYU";
        case TokenType::KW_MATTI: return "MATTI";
        case TokenType::KW_SATHYAM: return "SATHYAM";
        case TokenType::KW_ASATHYAM: return "ASATHYAM";
        case TokenType::KW_UM: return "UM";

        // ODLISH keywords
        case TokenType::KW_JADI: return "JADI";
        case TokenType::KW_JABAT: return "JABAT";
        case TokenType::KW_PHERA: return "PHERA";
        case TokenType::KW_SATA: return "SATA";
        case TokenType::KW_MITHYA: return "MITHYA";
        case TokenType::KW_EBAM: return "EBAM";

        // ASSLISH keywords
        case TokenType::KW_NOHOLE: return "NOHOLE";
        case TokenType::KW_JABOTLOI: return "JABOTLOI";
        case TokenType::KW_GHURAI: return "GHURAI";
        case TokenType::KW_MITHA: return "MITHA";
        case TokenType::KW_ARU: return "ARU";

        // MAITHLISH keywords
        case TokenType::KW_DEKHAOW: return "DEKHAOW";
        case TokenType::KW_JE: return "JE";
        case TokenType::KW_NAHI_TA: return "NAHI_TA";

        // KONKLISH keywords
        case TokenType::KW_DAKHOI: return "DAKHOI";
        case TokenType::KW_ZOR: return "ZOR";
        case TokenType::KW_NAITAR: return "NAITAR";
        case TokenType::KW_PAROT: return "PAROT";
        case TokenType::KW_VA: return "VA";

        // NEPLISH keywords
        case TokenType::KW_DEKHAU: return "DEKHAU";
        case TokenType::KW_YEDI: return "YEDI";
        case TokenType::KW_NABHAYE: return "NABHAYE";
        case TokenType::KW_JABSAMMA: return "JABSAMMA";
        case TokenType::KW_PHARKA: return "PHARKA";
        case TokenType::KW_SANO: return "SANO";
        case TokenType::KW_JHUTO: return "JHUTO";
        case TokenType::KW_RA: return "RA";
        case TokenType::KW_WA: return "WA";

        // SANGLISH keywords
        case TokenType::KW_DARSHAYA: return "DARSHAYA";
        case TokenType::KW_YADI: return "YADI";
        case TokenType::KW_ANYATHA: return "ANYATHA";
        case TokenType::KW_YAVAT: return "YAVAT";
        case TokenType::KW_KARYA: return "KARYA";
        case TokenType::KW_PRATYAGA: return "PRATYAGA";
        case TokenType::KW_CHA: return "CHA";

        // KASHLISH keywords
        case TokenType::KW_WAATAV: return "WAATAV";
        case TokenType::KW_GALAW: return "GALAW";

        // SINDLISH keywords
        case TokenType::KW_DIKHAY: return "DIKHAY";
        case TokenType::KW_KHOOTA: return "KHOOTA";
        case TokenType::KW_AIN: return "AIN";

        // DOGLISH keywords
        case TokenType::KW_WEKHO: return "WEKHO";
        case TokenType::KW_GER: return "GER";
        case TokenType::KW_NAHIN_TA: return "NAHIN_TA";

        // MANLISH keywords
        case TokenType::KW_NUNGSI: return "NUNGSI";
        case TokenType::KW_AMARE: return "AMARE";
        case TokenType::KW_NATTE: return "NATTE";
        case TokenType::KW_CHATLABA: return "CHATLABA";
        case TokenType::KW_THABAK: return "THABAK";
        case TokenType::KW_CHINGBA: return "CHINGBA";
        case TokenType::KW_THAMBAL: return "THAMBAL";
        case TokenType::KW_MAKHAL: return "MAKHAL";
        case TokenType::KW_AMASUNG: return "AMASUNG";
        case TokenType::KW_YANABA: return "YANABA";

        // BODLISH keywords
        case TokenType::KW_NANGI: return "NANGI";
        case TokenType::KW_BODO: return "BODO";
        case TokenType::KW_NANGOU: return "NANGOU";
        case TokenType::KW_JABSIM: return "JABSIM";
        case TokenType::KW_PHIRAVE: return "PHIRAVE";
        case TokenType::KW_SATO: return "SATO";
        case TokenType::KW_BEO: return "BEO";
        case TokenType::KW_AR: return "AR";
        case TokenType::KW_ABUA: return "ABUA";

        // SANTLISH keywords
        case TokenType::KW_DADO: return "DADO";
        case TokenType::KW_AMA: return "AMA";
        case TokenType::KW_BAE: return "BAE";
        case TokenType::KW_JABTO: return "JABTO";
        case TokenType::KW_TIKIN: return "TIKIN";
        case TokenType::KW_HON: return "HON";
        case TokenType::KW_BAE_HON: return "BAE_HON";
        case TokenType::KW_NOA: return "NOA";

        // English keywords
        case TokenType::KW_PRINT: return "PRINT";
        case TokenType::KW_IF: return "IF";
        case TokenType::KW_ELSE: return "ELSE";
        case TokenType::KW_WHILE: return "WHILE";
        case TokenType::KW_FOR: return "FOR";
        case TokenType::KW_FUNCTION: return "FUNCTION";
        case TokenType::KW_RETURN: return "RETURN";
        case TokenType::KW_TRUE: return "TRUE";
        case TokenType::KW_FALSE: return "FALSE";
        case TokenType::KW_AND: return "AND";
        case TokenType::KW_OR: return "OR";
        case TokenType::KW_NOT: return "NOT";
        case TokenType::KW_LET: return "LET";
        case TokenType::KW_FROM: return "FROM";
        case TokenType::KW_TO: return "TO";
        case TokenType::KW_THEN: return "THEN";
        case TokenType::KW_CREATE: return "CREATE";
        case TokenType::KW_CLASS: return "CLASS";
        case TokenType::KW_IMPORT: return "IMPORT";
        case TokenType::KW_BREAK: return "BREAK";
        case TokenType::KW_CONTINUE: return "CONTINUE";

        // Operators
        case TokenType::OP_PLUS: return "OP_PLUS";
        case TokenType::OP_MINUS: return "OP_MINUS";
        case TokenType::OP_MULTIPLY: return "OP_MULTIPLY";
        case TokenType::OP_DIVIDE: return "OP_DIVIDE";
        case TokenType::OP_MODULO: return "OP_MODULO";
        case TokenType::OP_ASSIGN: return "OP_ASSIGN";
        case TokenType::OP_EQUAL: return "OP_EQUAL";
        case TokenType::OP_NOT_EQUAL: return "OP_NOT_EQUAL";
        case TokenType::OP_LESS: return "OP_LESS";
        case TokenType::OP_LESS_EQUAL: return "OP_LESS_EQUAL";
        case TokenType::OP_GREATER: return "OP_GREATER";
        case TokenType::OP_GREATER_EQUAL: return "OP_GREATER_EQUAL";
        case TokenType::OP_AND: return "OP_AND";
        case TokenType::OP_OR: return "OP_OR";
        case TokenType::OP_NOT: return "OP_NOT";

        // Delimiters
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::COLON: return "COLON";
        case TokenType::ARROW: return "ARROW";
        case TokenType::DOUBLE_ARROW: return "DOUBLE_ARROW";

        // Special
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::ERROR_TOKEN: return "ERROR";

        default: return "UNKNOWN_TOKEN_TYPE";
    }
}

} // namespace mantra

#endif // MANTRA_TOKEN_H
