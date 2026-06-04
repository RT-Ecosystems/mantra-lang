
#include "http.h"
#include <curl/curl.h>
#include <sstream>
#include <iostream>
#include <memory>

namespace mantra {
namespace stdlib {

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static size_t HeaderCallback(char* buffer, size_t size, size_t nitems, std::vector<std::pair<std::string, std::string>>* headers) {
    std::string header(buffer, size * nitems);
    size_t colon = header.find(':');
    if (colon != std::string::npos) {
        std::string key = header.substr(0, colon);
        std::string value = header.substr(colon + 1);
        size_t start = value.find_first_not_of(" 	
");
        size_t end = value.find_last_not_of(" 	
");
        if (start != std::string::npos && end != std::string::npos) {
            headers->push_back({key, value.substr(start, end - start + 1)});
        }
    }
    return size * nitems;
}

class HttpClient {
public:
    HttpClient() { curl_global_init(CURL_GLOBAL_DEFAULT); }
    ~HttpClient() { curl_global_cleanup(); }

    struct Response {
        int status_code;
        std::string body;
        std::vector<std::pair<std::string, std::string>> headers;
        std::string error;
        bool success;
        Response() : status_code(0), success(false) {}
    };

    struct Request {
        std::string url;
        std::string method;
        std::string body;
        std::vector<std::pair<std::string, std::string>> headers;
        long timeout_ms;
        bool follow_redirects;
        Request() : method("GET"), timeout_ms(30000), follow_redirects(true) {}
    };

    Response send(const Request& req) {
        Response resp;
        CURL* curl = curl_easy_init();
        if (!curl) { resp.error = "Failed to initialize CURL"; return resp; }

        curl_easy_setopt(curl, CURLOPT_URL, req.url.c_str());
        if (req.method == "POST") curl_easy_setopt(curl, CURLOPT_POST, 1L);
        else if (req.method == "PUT") curl_easy_setopt(curl, CURLOPT_PUT, 1L);
        else if (req.method == "DELETE") curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

        if (!req.body.empty() && (req.method == "POST" || req.method == "PUT")) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.c_str());
        }

        struct curl_slist* header_list = nullptr;
        for (const auto& h : req.headers) {
            std::string header = h.first + ": " + h.second;
            header_list = curl_slist_append(header_list, header.c_str());
        }
        if (header_list) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp.body);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &resp.headers);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, req.timeout_ms);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, req.follow_redirects ? 1L : 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            resp.error = curl_easy_strerror(res);
        } else {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            resp.status_code = static_cast<int>(http_code);
            resp.success = (resp.status_code >= 200 && resp.status_code < 300);
        }

        if (header_list) curl_slist_free_all(header_list);
        curl_easy_cleanup(curl);
        return resp;
    }

    Response get(const std::string& url, const std::vector<std::pair<std::string, std::string>>& headers = {}) {
        Request req; req.url = url; req.method = "GET"; req.headers = headers; return send(req);
    }

    Response post(const std::string& url, const std::string& body, const std::vector<std::pair<std::string, std::string>>& headers = {}) {
        Request req; req.url = url; req.method = "POST"; req.body = body; req.headers = headers; return send(req);
    }

    Response put(const std::string& url, const std::string& body, const std::vector<std::pair<std::string, std::string>>& headers = {}) {
        Request req; req.url = url; req.method = "PUT"; req.body = body; req.headers = headers; return send(req);
    }

    Response del(const std::string& url, const std::vector<std::pair<std::string, std::string>>& headers = {}) {
        Request req; req.url = url; req.method = "DELETE"; req.headers = headers; return send(req);
    }
};

static std::unique_ptr<HttpClient> g_http_client;
HttpClient* getHttpClient() {
    if (!g_http_client) g_http_client = std::make_unique<HttpClient>();
    return g_http_client.get();
}

MantraValue builtinHttpGet(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING) throw std::runtime_error("http_get: URL string required");
    auto resp = getHttpClient()->get(args[0].string_value);
    if (!resp.success) return MantraValue::string("Error: " + resp.error);
    return MantraValue::string(resp.body);
}

MantraValue builtinHttpPost(const std::vector<MantraValue>& args) {
    if (args.size() < 1 || args[0].type != MantraValue::Type::STRING) throw std::runtime_error("http_post: URL string required");
    std::string body = (args.size() >= 2 && args[1].type == MantraValue::Type::STRING) ? args[1].string_value : "";
    auto resp = getHttpClient()->post(args[0].string_value, body);
    if (!resp.success) return MantraValue::string("Error: " + resp.error);
    return MantraValue::string(resp.body);
}

MantraValue builtinHttpPut(const std::vector<MantraValue>& args) {
    if (args.size() < 1 || args[0].type != MantraValue::Type::STRING) throw std::runtime_error("http_put: URL string required");
    std::string body = (args.size() >= 2 && args[1].type == MantraValue::Type::STRING) ? args[1].string_value : "";
    auto resp = getHttpClient()->put(args[0].string_value, body);
    if (!resp.success) return MantraValue::string("Error: " + resp.error);
    return MantraValue::string(resp.body);
}

MantraValue builtinHttpDelete(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING) throw std::runtime_error("http_delete: URL string required");
    auto resp = getHttpClient()->del(args[0].string_value);
    if (!resp.success) return MantraValue::string("Error: " + resp.error);
    return MantraValue::string(resp.body);
}

} // namespace stdlib
} // namespace mantra
