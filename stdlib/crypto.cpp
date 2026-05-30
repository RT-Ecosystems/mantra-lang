#include "crypto.h"
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <cstring>
#include <sstream>
#include <iomanip>
namespace mantra::stdlib {
MantraValue builtinAesEncrypt(const std::vector<MantraValue>& a){
    if(a.size() < 2) return MantraValue::string("");
    const std::string& key = a[0].string_value; const std::string& data = a[1].string_value;
    std::string out(data.size() + 16, 0);
    AES_KEY enc; AES_set_encrypt_key((const unsigned char*)key.c_str(), 128, &enc);
    for(size_t i=0; i<data.size(); i+=16) AES_encrypt((const unsigned char*)data.c_str()+i, (unsigned char*)&out[i], &enc);
    return MantraValue::string(out);
}
MantraValue builtinAesDecrypt(const std::vector<MantraValue>& a){
    if(a.size() < 2) return MantraValue::string("");
    const std::string& key = a[0].string_value; const std::string& data = a[1].string_value;
    std::string out(data.size(), 0);
    AES_KEY dec; AES_set_decrypt_key((const unsigned char*)key.c_str(), 128, &dec);
    for(size_t i=0; i<data.size(); i+=16) AES_decrypt((const unsigned char*)data.c_str()+i, (unsigned char*)&out[i], &dec);
    return MantraValue::string(out);
}
MantraValue builtinSha256(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("");
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)a[0].string_value.c_str(), a[0].string_value.size(), hash);
    std::ostringstream ss;
    for(int i=0; i<SHA256_DIGEST_LENGTH; ++i) ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return MantraValue::string(ss.str());
}
MantraValue builtinMd5(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("");
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((const unsigned char*)a[0].string_value.c_str(), a[0].string_value.size(), digest);
    std::ostringstream ss;
    for(int i=0; i<MD5_DIGEST_LENGTH; ++i) ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    return MantraValue::string(ss.str());
}
}
