#include "sqlite.h"
#include <sqlite3.h>
#include <cstdio>
namespace mantra::stdlib {
thread_local static sqlite3* g_db = nullptr;
MantraValue builtinSqlOpen(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::boolean(false);
    if(g_db) sqlite3_close(g_db);
    int rc = sqlite3_open(a[0].string_value.c_str(), &g_db);
    return MantraValue::boolean(rc == SQLITE_OK);
}
MantraValue builtinSqlExec(const std::vector<MantraValue>& a){
    if(a.empty() || !g_db) return MantraValue::string("");
    char* err = nullptr;
    int rc = sqlite3_exec(g_db, a[0].string_value.c_str(), nullptr, nullptr, &err);
    std::string res = (rc == SQLITE_OK) ? "OK" : (err ? err : "ERROR");
    if(err) sqlite3_free(err);
    return MantraValue::string(res);
}
MantraValue builtinSqlClose(const std::vector<MantraValue>&){
    if(g_db){ sqlite3_close(g_db); g_db = nullptr; return MantraValue::boolean(true); }
    return MantraValue::boolean(false);
}
}
