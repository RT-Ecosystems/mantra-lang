#include "random.h"
#include <random>
namespace mantra::stdlib {
static std::mt19937& rng() { static std::mt19937 gen(std::random_device{}()); return gen; }
MantraValue builtinRandom(const std::vector<MantraValue>&) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return MantraValue::number(dist(rng()));
}
MantraValue builtinRandomInt(const std::vector<MantraValue>& args) {
    int min = 0, max = 100;
    if (args.size() >= 1 && args[0].type == ValueType::Number) min = (int)args[0].number_value;
    if (args.size() >= 2 && args[1].type == ValueType::Number) max = (int)args[1].number_value;
    std::uniform_int_distribution<int> dist(min, max);
    return MantraValue::number(dist(rng()));
}
MantraValue builtinChoice(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != ValueType::Array || args[0].array_value.empty())
        return MantraValue::nullValue();
    std::uniform_int_distribution<size_t> dist(0, args[0].array_value.size()-1);
    return args[0].array_value[dist(rng())];
}
}
