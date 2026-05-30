#include "logging.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
namespace mantra::stdlib {
static std::string g_logfile = "mantra.log";
MantraValue builtinLogInfo(const std::vector<MantraValue>& a){
    std::ofstream out(g_logfile, std::ios::app);
    if(a.size()>=1) out << "[INFO] " << a[0].string_value << "\n";
    return MantraValue::boolean(true);
}
MantraValue builtinLogError(const std::vector<MantraValue>& a){
    std::ofstream out(g_logfile, std::ios::app);
    if(a.size()>=1) out << "[ERROR] " << a[0].string_value << "\n";
    return MantraValue::boolean(true);
}
MantraValue builtinLogWarn(const std::vector<MantraValue>& a){
    std::ofstream out(g_logfile, std::ios::app);
    if(a.size()>=1) out << "[WARN] " << a[0].string_value << "\n";
    return MantraValue::boolean(true);
}
MantraValue builtinLogSetFile(const std::vector<MantraValue>& a){
    if(a.size()>=1) g_logfile = a[0].string_value;
    return MantraValue::boolean(true);
}
}
