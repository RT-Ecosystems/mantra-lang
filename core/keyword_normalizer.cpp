#include "keyword_normalizer.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <climits>

namespace mantra {

// Central keyword mapping: all 22 languages + English keywords → TokenClass
// Total: ~250 keywords mapped to ~25 TokenClass entries
const std::unordered_map<std::string, TokenClass> KeywordNormalizer::KEYWORD_MAP = {
    // ===== PRINT KEYWORDS (19 languages) =====
    {"dikhao", TokenClass::PRINT},      // Hinglish
    {"kaado", TokenClass::PRINT},       // Tanglish
    {"dakho", TokenClass::PRINT},       // Punglish
    {"dekhao", TokenClass::PRINT},      // Benglish
    {"batavo", TokenClass::PRINT},      // Gujlish
    {"dakhav", TokenClass::PRINT},      // Marathlish
    {"chupinchu", TokenClass::PRINT},   // Telglish
    {"toro", TokenClass::PRINT},        // Kanglish
    {"kaaniku", TokenClass::PRINT},     // Manglish
    {"dekhaow", TokenClass::PRINT},     // Maithlish
    {"dakhoi", TokenClass::PRINT},      // Konklish
    {"dekhau", TokenClass::PRINT},      // Neplish
    {"darshaya", TokenClass::PRINT},    // Sanglish
    {"waatav", TokenClass::PRINT},      // Kashlish
    {"dikhay", TokenClass::PRINT},      // Sindlish
    {"wekho", TokenClass::PRINT},       // Doglish
    {"nungsi", TokenClass::PRINT},      // Manlish
    {"nangi", TokenClass::PRINT},       // Bodlish
    {"dado", TokenClass::PRINT},        // Santlish
    {"print", TokenClass::PRINT},       // English

    // ===== IF KEYWORDS (20 languages) =====
    {"agar", TokenClass::IF},           // Hinglish
    {"endral", TokenClass::IF},         // Tanglish
    {"jekr", TokenClass::IF},           // Punglish
    {"jodi", TokenClass::IF},           // Benglish
    {"jor", TokenClass::IF},            // Gujlish
    {"jar", TokenClass::IF},            // Marathlish
    {"ante", TokenClass::IF},           // Telglish
    {"agre", TokenClass::IF},           // Kanglish
    {"engil", TokenClass::IF},          // Manglish
    {"jadi", TokenClass::IF},           // Odlish
    {"nohole", TokenClass::IF},         // Asslish
    {"je", TokenClass::IF},             // Maithlish
    {"zor", TokenClass::IF},            // Konklish
    {"yedi", TokenClass::IF},           // Neplish
    {"yadi", TokenClass::IF},           // Sanglish
    {"galaw", TokenClass::IF},          // Kashlish
    {"ger", TokenClass::IF},            // Doglish
    {"amare", TokenClass::IF},          // Manlish
    {"bodo", TokenClass::IF},           // Bodlish
    {"ama", TokenClass::IF},            // Santlish
    {"if", TokenClass::IF},             // English

    // ===== ELSE KEYWORDS (21 languages) =====
    {"warna", TokenClass::ELSE},        // Hinglish
    {"illena", TokenClass::ELSE},       // Tanglish
    {"mudmudke", TokenClass::ELSE},     // Punglish
    {"nahole", TokenClass::ELSE},       // Benglish
    {"nahi_to", TokenClass::ELSE},      // Gujlish
    {"nahitar", TokenClass::ELSE},      // Marathlish
    {"lekapote", TokenClass::ELSE},     // Telglish
    {"illandre", TokenClass::ELSE},     // Kanglish
    {"allenkil", TokenClass::ELSE},     // Manglish
    {"bae", TokenClass::ELSE},          // Santlish
    {"nahi_ta", TokenClass::ELSE},      // Maithlish
    {"naitar", TokenClass::ELSE},       // Konklish
    {"nabhaye", TokenClass::ELSE},      // Neplish
    {"anyatha", TokenClass::ELSE},      // Sanglish
    {"nahin_ta", TokenClass::ELSE},     // Doglish
    {"natte", TokenClass::ELSE},        // Manlish
    {"nangou", TokenClass::ELSE},       // Bodlish
    {"else", TokenClass::ELSE},         // English

    // ===== WHILE KEYWORDS (14 languages) =====
    {"baarbaar", TokenClass::WHILE},    // Hinglish (note: also used for FOR in MANTRA)
    {"marumurai", TokenClass::WHILE},   // Tanglish
    {"murhke", TokenClass::WHILE},      // Punglish
    {"jabotokkhon", TokenClass::WHILE}, // Benglish
    {"jabtak", TokenClass::WHILE},      // Gujlish
    {"parat", TokenClass::WHILE},       // Marathlish
    {"andaka", TokenClass::WHILE},      // Telglish
    {"jotagje", TokenClass::WHILE},     // Kanglish
    {"varethum", TokenClass::WHILE},    // Manglish
    {"jabat", TokenClass::WHILE},       // Odlish
    {"jabotloi", TokenClass::WHILE},    // Asslish
    {"jabsamma", TokenClass::WHILE},    // Neplish
    {"yavat", TokenClass::WHILE},       // Sanglish
    {"jabsim", TokenClass::WHILE},      // Bodlish
    {"chatlaba", TokenClass::WHILE},    // Manlish
    {"while", TokenClass::WHILE},       // English

    // ===== FOR KEYWORDS =====
    {"baarbaar", TokenClass::FOR},      // Hinglish (special: both WHILE and FOR use this)
    {"for", TokenClass::FOR},           // English

    // ===== FUNCTION KEYWORDS (14 languages) =====
    {"kaam", TokenClass::FUNCTION},     // Hinglish
    {"seyal", TokenClass::FUNCTION},    // Tanglish
    {"te", TokenClass::FUNCTION},       // Punglish
    {"kaj", TokenClass::FUNCTION},      // Benglish
    {"pachhu", TokenClass::FUNCTION},   // Gujlish
    {"kharay", TokenClass::FUNCTION},   // Marathlish
    {"pani", TokenClass::FUNCTION},     // Telglish
    {"kelu", TokenClass::FUNCTION},     // Kanglish
    {"cheyu", TokenClass::FUNCTION},    // Manglish
    {"karya", TokenClass::FUNCTION},    // Sanglish
    {"phirave", TokenClass::FUNCTION},  // Bodlish
    {"tikin", TokenClass::FUNCTION},    // Santlish
    {"thabak", TokenClass::FUNCTION},   // Manlish
    {"function", TokenClass::FUNCTION}, // English

    // ===== RETURN KEYWORDS (12 languages) =====
    {"wapas", TokenClass::RETURN},      // Hinglish
    {"thirumbu", TokenClass::RETURN},   // Tanglish
    {"ja", TokenClass::RETURN},         // Punglish
    {"phrao", TokenClass::RETURN},      // Benglish
    {"tirigi", TokenClass::RETURN},     // Telglish
    {"hogi", TokenClass::RETURN},       // Kanglish
    {"matti", TokenClass::RETURN},      // Manglish
    {"pharka", TokenClass::RETURN},     // Neplish
    {"pratyaga", TokenClass::RETURN},   // Sanglish
    {"sato", TokenClass::RETURN},       // Bodlish
    {"chingba", TokenClass::RETURN},    // Manlish
    {"parot", TokenClass::RETURN},      // Konklish
    {"return", TokenClass::RETURN},     // English

    // ===== BREAK & CONTINUE =====
    {"break", TokenClass::BREAK},       // English (standard across all)
    {"continue", TokenClass::CONTINUE}, // English (standard across all)

    // ===== LET KEYWORDS (2 languages) =====
    {"rakho", TokenClass::LET},         // Hinglish
    {"let", TokenClass::LET},           // English

    // ===== TRUE KEYWORDS (14 languages) =====
    {"sach", TokenClass::TRUE},         // Hinglish
    {"unmai", TokenClass::TRUE},        // Tanglish
    {"satya", TokenClass::TRUE},        // Benglish
    {"sachu", TokenClass::TRUE},        // Gujlish
    {"khota", TokenClass::TRUE},        // Marathlish
    {"nijam", TokenClass::TRUE},        // Telglish
    {"nijha", TokenClass::TRUE},        // Kanglish
    {"sathyam", TokenClass::TRUE},      // Manglish
    {"sato", TokenClass::TRUE},         // Odlish
    {"thambal", TokenClass::TRUE},      // Manlish
    {"beo", TokenClass::TRUE},          // Bodlish
    {"bae_hon", TokenClass::TRUE},      // Santlish
    {"sano", TokenClass::TRUE},         // Neplish
    {"true", TokenClass::TRUE},         // English

    // ===== FALSE KEYWORDS (12 languages) =====
    {"jhooth", TokenClass::FALSE},      // Hinglish
    {"poi", TokenClass::FALSE},         // Tanglish
    {"mittha", TokenClass::FALSE},      // Benglish
    {"juthu", TokenClass::FALSE},       // Gujlish
    {"abaddham", TokenClass::FALSE},    // Telglish
    {"sullu", TokenClass::FALSE},       // Kanglish
    {"asathyam", TokenClass::FALSE},    // Manglish
    {"mithya", TokenClass::FALSE},      // Odlish
    {"makhal", TokenClass::FALSE},      // Manlish
    {"noa", TokenClass::FALSE},         // Santlish
    {"khoota", TokenClass::FALSE},      // Sindlish
    {"jhuto", TokenClass::FALSE},       // Neplish
    {"false", TokenClass::FALSE},       // English

    // ===== AND KEYWORDS (17 languages) =====
    {"aur", TokenClass::AND},           // Hinglish
    {"mattrum", TokenClass::AND},       // Tanglish
    {"ebong", TokenClass::AND},         // Benglish
    {"ane", TokenClass::AND},           // Gujlish
    {"ani", TokenClass::AND},           // Marathlish
    {"mariyu", TokenClass::AND},        // Telglish
    {"mattu", TokenClass::AND},         // Kanglish
    {"um", TokenClass::AND},            // Manglish
    {"ebam", TokenClass::AND},          // Odlish
    {"aru", TokenClass::AND},           // Asslish
    {"ra", TokenClass::AND},            // Neplish
    {"cha", TokenClass::AND},           // Sanglish
    {"ain", TokenClass::AND},           // Sindlish
    {"amasung", TokenClass::AND},       // Manlish
    {"ar", TokenClass::AND},            // Bodlish
    {"and", TokenClass::AND},           // English

    // ===== OR KEYWORDS (9 languages) =====
    {"ya", TokenClass::OR},             // Hinglish
    {"ke", TokenClass::OR},             // Gujlish
    {"kiva", TokenClass::OR},           // Marathlish
    {"leda", TokenClass::OR},           // Telglish
    {"athava", TokenClass::OR},         // Kanglish
    {"allathu", TokenClass::OR},        // Tanglish
    {"wa", TokenClass::OR},             // Neplish
    {"yanaba", TokenClass::OR},         // Manlish
    {"abua", TokenClass::OR},           // Bodlish
    {"or", TokenClass::OR},             // English

    // ===== NOT KEYWORDS (4 languages) =====
    {"nahi", TokenClass::NOT},          // Hinglish
    {"not", TokenClass::NOT},           // English
    {"!", TokenClass::NOT},             // Symbolic

    // ===== ASSIGNMENT & CONTROL =====
    {"then", TokenClass::THEN},         // English
    {"tab", TokenClass::THEN},          // Hinglish
    {"from", TokenClass::FROM},         // English
    {"se", TokenClass::FROM},           // Hinglish
    {"to", TokenClass::TO},             // English
    {"tak", TokenClass::TO},            // Hinglish
};

KeywordNormalizer::KeywordNormalizer(bool enableFuzzy)
    : enable_fuzzy_matching(enableFuzzy) {
}

TokenClass KeywordNormalizer::normalize(const std::string& keyword) const {
    auto it = KEYWORD_MAP.find(keyword);
    if (it != KEYWORD_MAP.end()) {
        return it->second;
    }
    return TokenClass::UNKNOWN;
}

std::string KeywordNormalizer::getCanonicalKeyword(TokenClass klass) const {
    if (!keywords_indexed) {
        buildKeywordIndex();
    }
    
    auto it = CLASS_TO_KEYWORDS.find(static_cast<int>(klass));
    if (it != CLASS_TO_KEYWORDS.end() && !it->second.empty()) {
        return it->second[0];  // Return first variant
    }
    return "";
}

std::vector<std::string> KeywordNormalizer::getVariants(TokenClass klass) const {
    if (!keywords_indexed) {
        buildKeywordIndex();
    }
    
    auto it = CLASS_TO_KEYWORDS.find(static_cast<int>(klass));
    if (it != CLASS_TO_KEYWORDS.end()) {
        return it->second;
    }
    return {};
}

void KeywordNormalizer::buildKeywordIndex() const {
    for (const auto& [keyword, klass] : KEYWORD_MAP) {
        CLASS_TO_KEYWORDS[static_cast<int>(klass)].push_back(keyword);
    }
    // Sort each class's keywords for consistency
    for (auto& [_, keywords] : CLASS_TO_KEYWORDS) {
        std::sort(keywords.begin(), keywords.end());
    }
    keywords_indexed = true;
}

int KeywordNormalizer::levenshteinDistance(const std::string& a, const std::string& b) {
    size_t alen = a.length();
    size_t blen = b.length();
    
    if (alen == 0) return blen;
    if (blen == 0) return alen;
    
    // Use dynamic programming to calculate edit distance
    std::vector<std::vector<int>> dp(alen + 1, std::vector<int>(blen + 1));
    
    for (size_t i = 0; i <= alen; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= blen; ++j) dp[0][j] = j;
    
    for (size_t i = 1; i <= alen; ++i) {
        for (size_t j = 1; j <= blen; ++j) {
            int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({
                dp[i - 1][j] + 1,      // deletion
                dp[i][j - 1] + 1,      // insertion
                dp[i - 1][j - 1] + cost // substitution
            });
        }
    }
    
    return dp[alen][blen];
}

std::optional<TokenClass> KeywordNormalizer::fuzzyMatch(const std::string& keyword) const {
    if (!enable_fuzzy_matching) {
        return std::nullopt;
    }
    return fuzzyMatchInternal(keyword);
}

std::optional<TokenClass> KeywordNormalizer::fuzzyMatchInternal(const std::string& keyword) const {
    if (!keywords_indexed) {
        buildKeywordIndex();
    }
    
    int bestDistance = INT_MAX;
    TokenClass bestMatch = TokenClass::UNKNOWN;
    
    // Calculate max allowed distance based on keyword length
    int maxDistance = std::max(1, static_cast<int>(keyword.length() / 3) + 1);
    
    for (const auto& [klass, variants] : CLASS_TO_KEYWORDS) {
        for (const auto& variant : variants) {
            int dist = levenshteinDistance(keyword, variant);
            if (dist < bestDistance && dist <= maxDistance) {
                bestDistance = dist;
                bestMatch = static_cast<TokenClass>(klass);
            }
        }
    }
    
    if (bestMatch != TokenClass::UNKNOWN) {
        return bestMatch;
    }
    return std::nullopt;
}

std::string KeywordNormalizer::getSuggestion(const std::string& misspelled) const {
    auto match = fuzzyMatch(misspelled);
    if (match) {
        std::string canonical = getCanonicalKeyword(match.value());
        return "Did you mean '" + canonical + "'?";
    }
    return "(no suggestions)";
}

UserLanguage KeywordNormalizer::detectLanguage(const std::string& keyword) const {
    // Detect language from keyword characteristics
    if (keyword == "dikhao" || keyword == "agar" || keyword == "aur" || keyword == "tab" || keyword == "wapas") {
        return UserLanguage::Hinglish;
    }
    if (keyword == "kaado" || keyword == "endral" || keyword == "mattrum" || keyword == "thirumbu") {
        return UserLanguage::Tanglish;
    }
    if (keyword == "dakho" || keyword == "jekr" || keyword == "te" || keyword == "ja") {
        return UserLanguage::Punglish;
    }
    if (keyword == "dekhao" || keyword == "jodi" || keyword == "ebong" || keyword == "phrao") {
        return UserLanguage::Benglish;
    }
    if (keyword == "batavo" || keyword == "jor" || keyword == "ane" || keyword == "nahi_to") {
        return UserLanguage::Gujlish;
    }
    if (keyword == "dakhav" || keyword == "jar" || keyword == "ani" || keyword == "kharay") {
        return UserLanguage::Marathlish;
    }
    if (keyword == "chupinchu" || keyword == "ante" || keyword == "mariyu" || keyword == "tirigi") {
        return UserLanguage::Telglish;
    }
    if (keyword == "toro" || keyword == "agre" || keyword == "mattu" || keyword == "hogi") {
        return UserLanguage::Kanglish;
    }
    if (keyword == "kaaniku" || keyword == "engil" || keyword == "um" || keyword == "matti") {
        return UserLanguage::Manglish;
    }
    if (keyword == "jadi" || keyword == "ebam") {
        return UserLanguage::Odlish;
    }
    if (keyword == "nohole" || keyword == "aru") {
        return UserLanguage::Asslish;
    }
    if (keyword == "dekhaow" || keyword == "je") {
        return UserLanguage::Maithlish;
    }
    if (keyword == "dakhoi" || keyword == "zor" || keyword == "va") {
        return UserLanguage::Konklish;
    }
    if (keyword == "dekhau" || keyword == "yedi" || keyword == "ra" || keyword == "wa") {
        return UserLanguage::Neplish;
    }
    if (keyword == "darshaya" || keyword == "yadi" || keyword == "cha") {
        return UserLanguage::Sanglish;
    }
    if (keyword == "waatav" || keyword == "galaw") {
        return UserLanguage::Kashlish;
    }
    if (keyword == "dikhay" || keyword == "khoota" || keyword == "ain") {
        return UserLanguage::Sindlish;
    }
    if (keyword == "wekho" || keyword == "ger") {
        return UserLanguage::Doglish;
    }
    if (keyword == "nungsi" || keyword == "amare" || keyword == "amasung") {
        return UserLanguage::Manlish;
    }
    if (keyword == "nangi" || keyword == "bodo" || keyword == "ar") {
        return UserLanguage::Bodlish;
    }
    if (keyword == "dado" || keyword == "ama" || keyword == "bae") {
        return UserLanguage::Santlish;
    }
    
    return UserLanguage::English;
}

std::string KeywordNormalizer::getLanguageName(UserLanguage lang) {
    switch (lang) {
        case UserLanguage::Hinglish: return "Hinglish (Hindi + English)";
        case UserLanguage::Tanglish: return "Tanglish (Tamil + English)";
        case UserLanguage::Punglish: return "Punglish (Punjabi + English)";
        case UserLanguage::Benglish: return "Benglish (Bengali + English)";
        case UserLanguage::Gujlish: return "Gujlish (Gujarati + English)";
        case UserLanguage::Marathlish: return "Marathlish (Marathi + English)";
        case UserLanguage::Telglish: return "Telglish (Telugu + English)";
        case UserLanguage::Kanglish: return "Kanglish (Kannada + English)";
        case UserLanguage::Manglish: return "Manglish (Malayalam + English)";
        case UserLanguage::Odlish: return "Odlish (Oriya + English)";
        case UserLanguage::Asslish: return "Asslish (Assamese + English)";
        case UserLanguage::Maithlish: return "Maithlish (Maithili + English)";
        case UserLanguage::Konklish: return "Konklish (Konkani + English)";
        case UserLanguage::Neplish: return "Neplish (Nepali + English)";
        case UserLanguage::Sanglish: return "Sanglish (Sanskrit + English)";
        case UserLanguage::Kashlish: return "Kashlish (Kashmiri + English)";
        case UserLanguage::Sindlish: return "Sindlish (Sindhi + English)";
        case UserLanguage::Doglish: return "Doglish (Dogri + English)";
        case UserLanguage::Manlish: return "Manlish (Manipuri + English)";
        case UserLanguage::Bodlish: return "Bodlish (Bodo + English)";
        case UserLanguage::Santlish: return "Santlish (Santhali + English)";
        case UserLanguage::English: return "English";
        default: return "Unknown";
    }
}

bool KeywordNormalizer::isValid(const std::string& keyword) const {
    return KEYWORD_MAP.find(keyword) != KEYWORD_MAP.end();
}

size_t KeywordNormalizer::getKeywordCount() const {
    return KEYWORD_MAP.size();
}

} // namespace mantra
