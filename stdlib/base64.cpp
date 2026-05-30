#include "base64.h"
static const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
namespace mantra::stdlib {
MantraValue builtinBase64Encode(const std::vector<MantraValue>& args) {
    if (args.empty()) return MantraValue::string("");
    const std::string& s = args[0].string_value;
    std::string out; int val = 0, valb = -6;
    for (unsigned char c : s) {
        val = (val << 8) + c; valb += 8;
        while (valb >= 0) { out.push_back(chars[(val>>valb)&0x3F]); valb -= 6; }
    }
    if (valb > -6) out.push_back(chars[((val<<8)>>(valb+8))&0x3F]);
    while (out.size() % 4) out.push_back('=');
    return MantraValue::string(out);
}
MantraValue builtinBase64Decode(const std::vector<MantraValue>& args) {
    if (args.empty()) return MantraValue::string("");
    const std::string& s = args[0].string_value;
    std::string out; std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[chars[i]] = i;
    int val = 0, valb = -8;
    for (unsigned char c : s) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c]; valb += 6;
        if (valb >= 0) { out.push_back(char((val>>valb)&0xFF)); valb -= 8; }
    }
    return MantraValue::string(out);
}
}
