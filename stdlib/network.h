#pragma once
#include <string>
#include <vector>

namespace mantra {
namespace stdlib {

struct HttpResponse {
    int status_code;
    std::string body;
    bool success;
};

HttpResponse http_get(const std::string& url);
HttpResponse http_post(const std::string& url,
                       const std::string& data);

} // namespace stdlib
} // namespace mantra
