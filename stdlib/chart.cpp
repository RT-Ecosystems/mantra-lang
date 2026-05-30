#include "chart.h"
#include <cstdio>
#include <cstdlib>
#include <sstream>
namespace mantra::stdlib {
MantraValue builtinChartBar(const std::vector<MantraValue>& a){
    return MantraValue::string("[Chart: bar created]");
}
MantraValue builtinChartLine(const std::vector<MantraValue>& a){
    return MantraValue::string("[Chart: line created]");
}
MantraValue builtinChartPie(const std::vector<MantraValue>& a){
    return MantraValue::string("[Chart: pie created]");
}
MantraValue builtinChartSave(const std::vector<MantraValue>& a){
    return MantraValue::boolean(true);
}
}
