#include "server.h"
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <thread>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

namespace mantra::stdlib {

// Global route storage
static std::map<std::string, std::string> routes;
static int server_socket = -1;

MantraValue builtinHttpServer(const std::vector<MantraValue>& args) {
    // httpServer(port) -> starts a simple HTTP server
    int port = 8080;
    if (!args.empty() && args[0].type == ValueType::Number) {
        port = static_cast<int>(args[0].number_value);
    }

    auto server_thread = std::thread([port]() {
        #ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        server_socket = sock;
        if (sock < 0) return;

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        bind(sock, (struct sockaddr*)&addr, sizeof(addr));
        listen(sock, 10);

        std::cout << "🌐 HTTP Server running on http://localhost:" << port << std::endl;

        while (true) {
            int client = accept(sock, nullptr, nullptr);
            if (client < 0) continue;

            char buffer[4096] = {0};
            recv(client, buffer, sizeof(buffer), 0);

            std::string request(buffer);
            std::string path = "/";
            size_t start = request.find("GET ") + 4;
            size_t end = request.find(" HTTP");
            if (start != std::string::npos && end != std::string::npos) {
                path = request.substr(start, end - start);
            }

            std::string response_body = "404 Not Found";
            std::string content_type = "text/html";

            if (routes.find(path) != routes.end()) {
                response_body = routes[path];
            } else if (routes.find("*") != routes.end()) {
                response_body = routes["*"];
            }

            std::ostringstream response;
            response << "HTTP/1.1 200 OK\r\n";
            response << "Content-Type: " << content_type << "\r\n";
            response << "Content-Length: " << response_body.length() << "\r\n";
            response << "Connection: close\r\n";
            response << "\r\n";
            response << response_body;

            std::string res = response.str();
            send(client, res.c_str(), res.length(), 0);

            #ifdef _WIN32
                closesocket(client);
            #else
                close(client);
            #endif
        }
    });

    server_thread.detach();
    return MantraValue::boolean(true);
}

MantraValue builtinHttpRoute(const std::vector<MantraValue>& args) {
    // httpRoute(path, content) -> adds a route
    if (args.size() >= 2 && args[0].type == ValueType::String && args[1].type == ValueType::String) {
        routes[args[0].string_value] = args[1].string_value;
        return MantraValue::boolean(true);
    }
    return MantraValue::boolean(false);
}

MantraValue builtinHttpListen(const std::vector<MantraValue>& args) {
    return builtinHttpServer(args);
}

MantraValue builtinHttpResponse(const std::vector<MantraValue>& args) {
    // httpResponse(content) -> returns content (utility for route handlers)
    if (!args.empty() && args[0].type == ValueType::String) {
        return MantraValue::string(args[0].string_value);
    }
    return MantraValue::string("");
}

} // namespace mantra::stdlib
