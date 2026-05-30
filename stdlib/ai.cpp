#include "ai.h"
#include "../error/error.h"
#include <cmath>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <iostream>

// ==================== SHA-256 (public domain) ====================
namespace {
    const uint32_t SHA256_K[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };

    uint32_t rotr(uint32_t x, uint32_t n) { return (x >> n) | (x << (32 - n)); }
    uint32_t sha2_ch(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (~x & z); }
    uint32_t sha2_maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (x & z) ^ (y & z); }
    uint32_t sha2_S0(uint32_t x) { return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22); }
    uint32_t sha2_S1(uint32_t x) { return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25); }
    uint32_t sha2_s0(uint32_t x) { return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3); }
    uint32_t sha2_s1(uint32_t x) { return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10); }

    std::string sha256(const std::string& data) {
        uint32_t h[8] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
        std::vector<uint8_t> msg(data.begin(), data.end());
        uint64_t bitlen = msg.size() * 8;
        msg.push_back(0x80);
        while ((msg.size() % 64) != 56) msg.push_back(0);
        for (int i=7; i>=0; --i) msg.push_back((bitlen >> (i*8)) & 0xff);
        for (size_t offset=0; offset<msg.size(); offset+=64) {
            uint32_t w[64];
            for (int i=0; i<16; ++i)
                w[i] = (uint32_t(msg[offset+i*4])<<24) | (uint32_t(msg[offset+i*4+1])<<16) |
                       (uint32_t(msg[offset+i*4+2])<<8) | uint32_t(msg[offset+i*4+3]);
            for (int i=16; i<64; ++i)
                w[i] = sha2_s1(w[i-2]) + w[i-7] + sha2_s0(w[i-15]) + w[i-16];
            uint32_t a=h[0], b=h[1], c=h[2], d=h[3], e=h[4], f=h[5], g=h[6], H=h[7];
            for (int i=0; i<64; ++i) {
                uint32_t T1 = H + sha2_S1(e) + sha2_ch(e,f,g) + SHA256_K[i] + w[i];
                uint32_t T2 = sha2_S0(a) + sha2_maj(a,b,c);
                H=g; g=f; f=e; e=d+T1; d=c; c=b; b=a; a=T1+T2;
            }
            h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=H;
        }
        std::ostringstream oss;
        for (int i=0; i<8; ++i) oss << std::hex << std::setfill('0') << std::setw(8) << h[i];
        return oss.str();
    }
}

// ==================== Vector utilities ====================
namespace {
    using Vector = std::vector<double>;

    Vector textToVector(const std::string& text, int dim = 1024) {
        Vector vec(dim, 0.0);
        std::string hash = sha256(text);
        for (size_t i = 0; i < hash.size() && i < dim; ++i) {
            vec[i] = (static_cast<double>(hash[i]) / 255.0) * 2.0 - 1.0;
        }
        double norm = 0.0;
        for (double v : vec) norm += v*v;
        norm = std::sqrt(norm);
        if (norm > 1e-8) for (double& v : vec) v /= norm;
        return vec;
    }

    double cosineSimilarity(const Vector& a, const Vector& b) {
        double dot = 0.0, na = 0.0, nb = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            dot += a[i] * b[i];
            na += a[i] * a[i];
            nb += b[i] * b[i];
        }
        if (na < 1e-12 || nb < 1e-12) return 0.0;
        return dot / (std::sqrt(na) * std::sqrt(nb));
    }
}

// ==================== Gyan (Knowledge Base) ====================
struct Gyan {
    std::vector<std::pair<std::string, Vector>> entries;
    int dim;
    Gyan(int d = 1024) : dim(d) {}
};

// ==================== Vidyarthi (Student Model) ====================
struct Vidyarthi {
    std::shared_ptr<Gyan> gyan;
    Vidyarthi() : gyan(std::make_shared<Gyan>()) {}
    void learn(const std::string& sentence);
    std::string ask(const std::string& question) const;
};

void Vidyarthi::learn(const std::string& sentence) {
    Vector vec = textToVector(sentence, gyan->dim);
    for (const auto& entry : gyan->entries) {
        if (cosineSimilarity(vec, entry.second) > 0.95) return;  // duplicate
    }
    gyan->entries.push_back({sentence, vec});
}

std::string Vidyarthi::ask(const std::string& question) const {
    if (gyan->entries.empty()) return "[कुछ सीखा नहीं गया]";
    Vector qvec = textToVector(question, gyan->dim);
    double best_sim = -1.0;
    std::string best_text;
    for (const auto& entry : gyan->entries) {
        double sim = cosineSimilarity(qvec, entry.second);
        if (sim > best_sim) {
            best_sim = sim;
            best_text = entry.first;
        }
    }
    return best_text;
}

// ==================== Model save/load ====================
bool saveModel(const Vidyarthi& vid, const std::string& path) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    int dim = vid.gyan->dim;
    int n = vid.gyan->entries.size();
    out.write(reinterpret_cast<const char*>(&dim), sizeof(dim));
    out.write(reinterpret_cast<const char*>(&n), sizeof(n));
    for (const auto& e : vid.gyan->entries) {
        int len = e.first.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(e.first.data(), len);
        out.write(reinterpret_cast<const char*>(e.second.data()), dim * sizeof(double));
    }
    return out.good();
}

bool loadModel(Vidyarthi& vid, const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;
    int dim, n;
    in.read(reinterpret_cast<char*>(&dim), sizeof(dim));
    in.read(reinterpret_cast<char*>(&n), sizeof(n));
    vid.gyan->dim = dim;
    vid.gyan->entries.clear();
    for (int i=0; i<n; ++i) {
        int len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        std::string text(len, '\0');
        in.read(&text[0], len);
        Vector vec(dim);
        in.read(reinterpret_cast<char*>(vec.data()), dim * sizeof(double));
        vid.gyan->entries.push_back({text, vec});
    }
    return in.good();
}

// ==================== Global Vidyarthi instance for built-in functions ====================
static std::shared_ptr<Vidyarthi> globalVidyarthi = nullptr;

std::shared_ptr<Vidyarthi> getOrCreateGlobalVidyarthi() {
    if (!globalVidyarthi) {
        globalVidyarthi = std::make_shared<Vidyarthi>();
    }
    return globalVidyarthi;
}

// ==================== Built-in wrappers ====================
namespace mantra::stdlib {

    MantraValue builtinBeejCreate(const std::vector<MantraValue>& args) {
        int dim = 1024;
        if (!args.empty() && args[0].type == ValueType::Number)
            dim = static_cast<int>(args[0].number_value);
        globalVidyarthi = std::make_shared<Vidyarthi>();
        globalVidyarthi->gyan->dim = dim;
        return MantraValue::string("beej_ready");
    }

    MantraValue builtinGyanCreate(const std::vector<MantraValue>& args) {
        auto g = std::make_shared<Gyan>();
        if (!args.empty() && args[0].type == ValueType::Number)
            g->dim = static_cast<int>(args[0].number_value);
        globalVidyarthi = std::make_shared<Vidyarthi>();
        globalVidyarthi->gyan = g;
        return MantraValue::string("gyan_ready");
    }

    MantraValue builtinVidyarthiCreate(const std::vector<MantraValue>& args) {
        globalVidyarthi = std::make_shared<Vidyarthi>();
        return MantraValue::string("vidyarthi_ready");
    }

    MantraValue builtinSikho(const std::vector<MantraValue>& args) {
        if (args.size() < 1) return MantraValue::boolean(false);
        auto vid = getOrCreateGlobalVidyarthi();
        vid->learn(args[0].string_value);
        return MantraValue::boolean(true);
    }

    MantraValue builtinPuchho(const std::vector<MantraValue>& args) {
        if (args.size() < 1) return MantraValue::string("");
        auto vid = getOrCreateGlobalVidyarthi();
        return MantraValue::string(vid->ask(args[0].string_value));
    }

    MantraValue builtinHash(const std::vector<MantraValue>& args) {
        if (args.empty()) return MantraValue::string("");
        return MantraValue::string(sha256(args[0].toString()));
    }

    MantraValue builtinSimilar(const std::vector<MantraValue>& args) {
        if (args.size() < 2) return MantraValue::number(0.0);
        Vector a = textToVector(args[0].toString());
        Vector b = textToVector(args[1].toString());
        return MantraValue::number(cosineSimilarity(a, b));
    }

    MantraValue builtinSaveModel(const std::vector<MantraValue>& args) {
        if (args.size() < 1) return MantraValue::boolean(false);
        auto vid = getOrCreateGlobalVidyarthi();
        bool ok = saveModel(*vid, args[0].string_value);
        return MantraValue::boolean(ok);
    }

    MantraValue builtinLoadModel(const std::vector<MantraValue>& args) {
        if (args.size() < 1) return MantraValue::boolean(false);
        auto vid = getOrCreateGlobalVidyarthi();
        bool ok = loadModel(*vid, args[0].string_value);
        return MantraValue::boolean(ok);
    }

} // namespace mantra::stdlib
