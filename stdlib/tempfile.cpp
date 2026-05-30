#include "tempfile.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
namespace mantra::stdlib {
MantraValue builtinTempFile(const std::vector<MantraValue>&){char t[]="/tmp/mantra_XXXXXX";int fd=mkstemp(t);if(fd==-1)return MantraValue::string("");close(fd);return MantraValue::string(t);}
}
