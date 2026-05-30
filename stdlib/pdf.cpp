#include "pdf.h"
#include <cstdio>
#include <cstdlib>
#include <sstream>
namespace mantra::stdlib {
MantraValue builtinPdfCreate(const std::vector<MantraValue>& a){
    return MantraValue::boolean(true);
}
MantraValue builtinPdfText(const std::vector<MantraValue>& a){
    return MantraValue::boolean(true);
}
MantraValue builtinPdfImage(const std::vector<MantraValue>& a){
    return MantraValue::boolean(true);
}
MantraValue builtinPdfSave(const std::vector<MantraValue>& a){
    return MantraValue::boolean(true);
}
}
