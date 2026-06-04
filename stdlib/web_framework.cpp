#include "web_framework.h"
#include "server.h"
#include <regex>
#include <sstream>

namespace mantra {
namespace stdlib {

struct WebRoute {
    std::string method;
    std::string path;
    RouteHandler handler;
    std::regex pattern;
    std::vector<std::string> param_names;

    WebRoute(const std::string& m, const std::string& p, RouteHandler h) : method(m), handler(h) {
        std::string regex_str = "^";
        size_t pos = 0;
        while (pos < p.size()) {
            if (p[pos] == ':' && pos + 1 < p.size()) {
                size_t end = pos + 1;
                while (end < p.size() && p[end] != '/' && p[end] != '?') end++;
                param_names.push_back(p.substr(pos + 1, end - pos - 1));
                regex_str += "([^/]+)";
                pos = end;
            } else if (p[pos] == '*') {
                regex_str += ".*";
                pos++;
            } else {
                regex_str += p[pos];
                pos++;
            }
        }
        regex_str += "$";
        pattern = std::regex(regex_str);
        path = p;
    }
};

class WebApp {
public:
    WebApp() : server_(8080) {}
    WebApp(int port) : server_(port) {}

    void get(const std::string& path, RouteHandler handler) { routes_.emplace_back("GET", path, handler); }
    void post(const std::string& path, RouteHandler handler) { routes_.emplace_back("POST", path, handler); }
    void put(const std::string& path, RouteHandler handler) { routes_.emplace_back("PUT", path, handler); }
    void del(const std::string& path, RouteHandler handler) { routes_.emplace_back("DELETE", path, handler); }
    void patch(const std::string& path, RouteHandler handler) { routes_.emplace_back("PATCH", path, handler); }
    void use(Middleware middleware) { middlewares_.push_back(middleware); }

    void staticFiles(const std::string& url_path, const std::string& dir_path) {
        get(url_path + "/*", [dir_path](const HttpRequest& req) -> HttpResponse {
            HttpResponse resp;
            std::string filepath = dir_path + req.path.substr(url_path.size());
            std::ifstream file(filepath);
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                resp.body = buffer.str();
                if (filepath.ends_with(".html")) resp.headers["Content-Type"] = "text/html";
                else if (filepath.ends_with(".css")) resp.headers["Content-Type"] = "text/css";
                else if (filepath.ends_with(".js")) resp.headers["Content-Type"] = "application/javascript";
                else if (filepath.ends_with(".json")) resp.headers["Content-Type"] = "application/json";
                else resp.headers["Content-Type"] = "text/plain";
            } else {
                resp.status_code = 404;
                resp.body = "File not found: " + filepath;
            }
            return resp;
        });
    }

    void start() {
        for (const auto& route : routes_) {
            server_.route(route.method, route.path, [this, route](const HttpRequest& req) -> HttpResponse {
                return handleRequest(req, route);
            });
        }
        server_.start();
    }

    void stop() { server_.stop(); }

private:
    HttpServer server_;
    std::vector<WebRoute> routes_;
    std::vector<Middleware> middlewares_;

    HttpResponse handleRequest(const HttpRequest& req, const WebRoute& route) {
        HttpResponse resp;
        std::smatch match;
        std::regex_match(req.path, match, route.pattern);
        std::map<std::string, std::string> params;
        for (size_t i = 0; i < route.param_names.size() && i + 1 < match.size(); i++) {
            params[route.param_names[i]] = match[i + 1].str();
        }
        HttpRequest modified_req = req;
        for (const auto& [key, value] : params) modified_req.query_params[key] = value;
        for (auto& mw : middlewares_) mw(modified_req, resp);
        return route.handler(modified_req);
    }
};

static std::unique_ptr<WebApp> g_web_app;

WebApp* getWebApp() {
    if (!g_web_app) g_web_app = std::make_unique<WebApp>();
    return g_web_app.get();
}

MantraValue builtinWebGet(const std::vector<MantraValue>& args) {
    if (args.size() < 2 || args[0].type != MantraValue::Type::STRING || args[1].type != MantraValue::Type::FUNCTION) {
        throw std::runtime_error("web_get: path and handler required");
    }
    getWebApp()->get(args[0].string_value, [](const HttpRequest& req) -> HttpResponse {
        HttpResponse resp;
        resp.body = "Hello from MANTRA Web Framework!";
        return resp;
    });
    return MantraValue::string("Route added: GET " + args[0].string_value);
}

MantraValue builtinWebPost(const std::vector<MantraValue>& args) {
    if (args.size() < 2 || args[0].type != MantraValue::Type::STRING || args[1].type != MantraValue::Type::FUNCTION) {
        throw std::runtime_error("web_post: path and handler required");
    }
    getWebApp()->post(args[0].string_value, [](const HttpRequest& req) -> HttpResponse {
        HttpResponse resp;
        resp.body = "POST received";
        return resp;
    });
    return MantraValue::string("Route added: POST " + args[0].string_value);
}

MantraValue builtinWebStatic(const std::vector<MantraValue>& args) {
    if (args.size() < 2 || args[0].type != MantraValue::Type::STRING || args[1].type != MantraValue::Type::STRING) {
        throw std::runtime_error("web_static: url_path and dir_path required");
    }
    getWebApp()->staticFiles(args[0].string_value, args[1].string_value);
    return MantraValue::string("Static files: " + args[0].string_value + " -> " + args[1].string_value);
}

MantraValue builtinWebStart(const std::vector<MantraValue>& args) {
    int port = 8080;
    if (!args.empty() && args[0].type == MantraValue::Type::NUMBER) port = static_cast<int>(args[0].number_value);
    if (!g_web_app) g_web_app = std::make_unique<WebApp>(port);
    std::thread([]() { g_web_app->start(); }).detach();
    return MantraValue::string("Web server started on port " + std::to_string(port));
}

MantraValue builtinWebStop(const std::vector<MantraValue>& args) {
    if (g_web_app) { g_web_app->stop(); return MantraValue::string("Web server stopped"); }
    return MantraValue::string("Web server not running");
}

MantraValue builtinJsonResponse(const std::vector<MantraValue>& args) {
    if (args.empty()) throw std::runtime_error("json_response: data required");
    HttpResponse resp;
    resp.status_code = 200;
    resp.headers["Content-Type"] = "application/json";
    resp.body = "{\"status\":\"ok\"}";
    MantraValue result = MantraValue::object();
    result.object_value["__type__"] = MantraValue::string("response");
    result.object_value["status"] = MantraValue::number(resp.status_code);
    result.object_value["body"] = MantraValue::string(resp.body);
    return result;
}

MantraValue builtinHtmlResponse(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING) throw std::runtime_error("html_response: html string required");
    HttpResponse resp;
    resp.status_code = 200;
    resp.headers["Content-Type"] = "text/html";
    resp.body = args[0].string_value;
    MantraValue result = MantraValue::object();
    result.object_value["__type__"] = MantraValue::string("response");
    result.object_value["status"] = MantraValue::number(200);
    result.object_value["body"] = MantraValue::string(resp.body);
    return result;
}

} // namespace stdlib
} // namespace mantra