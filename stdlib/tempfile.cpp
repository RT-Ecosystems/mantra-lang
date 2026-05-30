#include "tempfile.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
namespace mantra::stdlib {
MantraValue builtinTempFile(const std::vector<MantraValue>&) {
    char templ[] = "/tmp/mantra_XXXXXX";
    int fd = mkstemp(templ);
    if (fd == -1) return MantraValue::string("");
    close(fd);
    return MantraValue::string(templ);
}
}
