#include "../core/interpreter.h"
#include "network.h"
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <sstream>

namespace mantra {
namespace stdlib {

// Simple HTTP using curl command (no extra deps)
static std::string exec_command(const std::string& cmd) {
    std::array<char, 512> buf;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)>
        pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "";
    while (fgets(buf.data(), buf.size(), pipe.get()))
        result += buf.data();
    return result;
}

HttpResponse http_get(const std::string& url) {
    HttpResponse resp;
    // Get status code
    std::string status_cmd = "curl -s -o /dev/null -w \"%{http_code}\" \""
        + url + "\" 2>/dev/null";
    std::string status_str = exec_command(status_cmd);
    try { resp.status_code = std::stoi(status_str); }
    catch (...) { resp.status_code = 0; }

    // Get body
    std::string body_cmd = "curl -s \"" + url + "\" 2>/dev/null";
    resp.body = exec_command(body_cmd);
    resp.success = (resp.status_code >= 200 && resp.status_code < 300);
    return resp;
}

HttpResponse http_post(const std::string& url,
                       const std::string& data) {
    HttpResponse resp;
    std::string status_cmd = "curl -s -o /dev/null -w \"%{http_code}\" "
        "-X POST -d \"" + data + "\" \"" + url + "\" 2>/dev/null";
    std::string status_str = exec_command(status_cmd);
    try { resp.status_code = std::stoi(status_str); }
    catch (...) { resp.status_code = 0; }

    std::string body_cmd = "curl -s -X POST -d \"" + data +
        "\" \"" + url + "\" 2>/dev/null";
    resp.body = exec_command(body_cmd);
    resp.success = (resp.status_code >= 200 && resp.status_code < 300);
    return resp;
}

MantraValue builtinHttpGet(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING)
        throw std::runtime_error("maango/fetch: URL string chahiye");
    auto resp = http_get(args[0].string_value);
    // Return as string (body)
    return MantraValue::string(resp.body);
}

MantraValue builtinHttpPost(const std::vector<MantraValue>& args) {
    if (args.size() < 1 || args[0].type != MantraValue::Type::STRING)
        throw std::runtime_error("bhejo/post: URL string chahiye");
    std::string data = (args.size() >= 2 &&
        args[1].type == MantraValue::Type::STRING)
        ? args[1].string_value : "";
    auto resp = http_post(args[0].string_value, data);
    return MantraValue::string(resp.body);
}

// Simple JSON string extraction (basic, no full parse)
MantraValue builtinJsonParse(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING)
        throw std::runtime_error("json_padho: string chahiye");
    // Return as-is string (full JSON parse needs a library)
    return MantraValue::string(args[0].string_value);
}

MantraValue builtinJsonStringify(const std::vector<MantraValue>& args) {
    if (args.empty())
        return MantraValue::string("null");
    // Convert MantraValue to simple JSON
    const auto& v = args[0];
    if (v.type == MantraValue::Type::STRING)
        return MantraValue::string("\"" + v.string_value + "\"");
    if (v.type == MantraValue::Type::NUMBER) {
        std::ostringstream ss;
        ss << v.number_value;
        return MantraValue::string(ss.str());
    }
    if (v.type == MantraValue::Type::BOOLEAN)
        return MantraValue::string(v.bool_value ? "true" : "false");
    return MantraValue::string("null");
}

} // namespace stdlib
} // namespace mantra
