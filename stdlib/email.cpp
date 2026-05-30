#include "email.h"
#include <cstdio>
#include <cstdlib>
#include <sstream>
namespace mantra::stdlib {
MantraValue builtinEmailSend(const std::vector<MantraValue>& a){
    return MantraValue::boolean(true);
}
MantraValue builtinEmailSetServer(const std::vector<MantraValue>& a){
    return MantraValue::boolean(true);
}
}
