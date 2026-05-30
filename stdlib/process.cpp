#include "process.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
namespace mantra::stdlib {
MantraValue builtinProcessRun(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::number(-1);
    pid_t pid = fork();
    if(pid == 0){ execl("/bin/sh", "sh", "-c", a[0].string_value.c_str(), nullptr); _exit(1); }
    return MantraValue::number(pid);
}
MantraValue builtinProcessOutput(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::string("");
    std::string cmd = a[0].string_value + " 2>&1";
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe) return MantraValue::string("");
    char buf[256]; std::string result;
    while(fgets(buf, sizeof(buf), pipe)) result += buf;
    pclose(pipe);
    return MantraValue::string(result);
}
MantraValue builtinProcessWait(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::number(-1);
    int status; waitpid((pid_t)a[0].number_value, &status, 0);
    return MantraValue::number(WEXITSTATUS(status));
}
MantraValue builtinProcessKill(const std::vector<MantraValue>& a){
    if(a.empty()) return MantraValue::boolean(false);
    return MantraValue::boolean(kill((pid_t)a[0].number_value, SIGTERM) == 0);
}
}
