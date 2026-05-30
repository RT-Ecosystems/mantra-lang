#include "hash.h"
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
#include <cstring>
namespace mantra::stdlib {
MantraValue builtinSha1(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("");
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char*)a[0].string_value.c_str(), a[0].string_value.size(), hash);
    std::ostringstream ss;
    for(int i=0; i<SHA_DIGEST_LENGTH; ++i) ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return MantraValue::string(ss.str());
}
MantraValue builtinSha512(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("");
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512((const unsigned char*)a[0].string_value.c_str(), a[0].string_value.size(), hash);
    std::ostringstream ss;
    for(int i=0; i<SHA512_DIGEST_LENGTH; ++i) ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return MantraValue::string(ss.str());
}
MantraValue builtinCrc32(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::number(0);
    unsigned int crc = 0xFFFFFFFF;
    for(char c : a[0].string_value){
        crc ^= (unsigned char)c;
        for(int i=0; i<8; ++i) crc = (crc>>1) ^ ((crc&1) ? 0xEDB88320 : 0);
    }
    return MantraValue::number(crc ^ 0xFFFFFFFF);
}
}
