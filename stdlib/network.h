#pragma once
#include "../core/interpreter.h"
#include <string>

namespace mantra {
namespace stdlib {

// HTTP response structure
struct HttpResponse {
    int status_code;
    std::string body;
    bool success;
};

// Core HTTP functions
HttpResponse http_get(const std::string& url);
HttpResponse http_post(const std::string& url,
                       const std::string& data);

// Register all network functions in interpreter
void registerNetworkFunctions(
    std::unordered_map<std::string,
        std::function<MantraValue(
            const std::vector<MantraValue>&)>>& funcs);

// Built-in functions
MantraValue builtinHttpGet(const std::vector<MantraValue>& args);
MantraValue builtinHttpPost(const std::vector<MantraValue>& args);
MantraValue builtinJsonParse(const std::vector<MantraValue>& args);
MantraValue builtinJsonStringify(const std::vector<MantraValue>& args);

} // namespace stdlib
} // namespace mantra
