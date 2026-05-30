#include "gui.h"
#include <cstdio>
#include <cstdlib>
namespace mantra::stdlib {
MantraValue builtinGuiWindow(const std::vector<MantraValue>& a){
    return MantraValue::boolean(true);
}
MantraValue builtinGuiButton(const std::vector<MantraValue>& a){
    return MantraValue::boolean(true);
}
MantraValue builtinGuiLabel(const std::vector<MantraValue>& a){
    return MantraValue::boolean(true);
}
MantraValue builtinGuiRun(const std::vector<MantraValue>&){
    return MantraValue::boolean(true);
}
}
