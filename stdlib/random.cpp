#include "random.h"
#include <random>
namespace mantra::stdlib {
static std::mt19937& rng(){static std::mt19937 g(std::random_device{}());return g;}
MantraValue builtinRandom(const std::vector<MantraValue>&){std::uniform_real_distribution<double> d(0,1);return MantraValue::number(d(rng()));}
MantraValue builtinRandomInt(const std::vector<MantraValue>& a){int x=0,y=100;if(a.size()>=1&&a[0].type==ValueType::Number)x=(int)a[0].number_value;if(a.size()>=2&&a[1].type==ValueType::Number)y=(int)a[1].number_value;std::uniform_int_distribution<int> d(x,y);return MantraValue::number(d(rng()));}
MantraValue builtinChoice(const std::vector<MantraValue>& a){if(a.empty()||a[0].type!=ValueType::Array||a[0].array_value.empty())return MantraValue::nullValue();std::uniform_int_distribution<size_t> d(0,a[0].array_value.size()-1);return a[0].array_value[d(rng())];}
}
