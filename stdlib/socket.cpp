#include "socket.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
namespace mantra::stdlib {
thread_local static int g_sock = -1;
MantraValue builtinTcpConnect(const std::vector<MantraValue>& a){
    if(a.size() < 2) return MantraValue::boolean(false);
    if(g_sock >= 0) close(g_sock);
    g_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(g_sock < 0) return MantraValue::boolean(false);
    struct sockaddr_in addr; memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET; addr.sin_port = htons((int)a[1].number_value);
    inet_pton(AF_INET, a[0].string_value.c_str(), &addr.sin_addr);
    return MantraValue::boolean(connect(g_sock, (struct sockaddr*)&addr, sizeof(addr)) == 0);
}
MantraValue builtinTcpSend(const std::vector<MantraValue>& a){
    if(a.empty() || g_sock < 0) return MantraValue::boolean(false);
    const std::string& s = a[0].string_value;
    return MantraValue::boolean(send(g_sock, s.c_str(), s.size(), 0) > 0);
}
MantraValue builtinTcpRecv(const std::vector<MantraValue>& a){
    if(g_sock < 0) return MantraValue::string("");
    int maxlen = 4096; if(!a.empty()) maxlen = (int)a[0].number_value;
    char* buf = new char[maxlen+1];
    int n = recv(g_sock, buf, maxlen, 0);
    if(n <= 0){ delete[] buf; return MantraValue::string(""); }
    buf[n] = 0; std::string res(buf); delete[] buf;
    return MantraValue::string(res);
}
MantraValue builtinTcpClose(const std::vector<MantraValue>&){
    if(g_sock >= 0){ close(g_sock); g_sock = -1; return MantraValue::boolean(true); }
    return MantraValue::boolean(false);
}
MantraValue builtinTcpListen(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::number(-1);
    int port = (int)a[0].number_value;
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if(s < 0) return MantraValue::number(-1);
    struct sockaddr_in addr; memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET; addr.sin_port = htons(port); addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0 || listen(s, 5) < 0){ close(s); return MantraValue::number(-1); }
    return MantraValue::number(s);
}
}
