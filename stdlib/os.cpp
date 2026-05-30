#include "os.h"
#include <cstdlib>
namespace mantra::stdlib {
MantraValue builtinGetEnv(const std::vector<MantraValue>& a){if(a.empty())return MantraValue::string("");const char*v=std::getenv(a[0].string_value.c_str());return MantraValue::string(v?v:"");}
MantraValue builtinSystem(const std::vector<MantraValue>& a){if(a.empty())return MantraValue::number(-1);return MantraValue::number(std::system(a[0].string_value.c_str()));}
}
