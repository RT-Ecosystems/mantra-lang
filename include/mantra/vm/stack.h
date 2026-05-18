#pragma once

#include "value.h"

#include <cstddef>
#include <vector>

namespace mantra {

class Stack {
public:
    void push(const Value& value);
    Value pop();
    const Value& peek(std::size_t distance = 0) const;

    std::size_t size() const;
    bool empty() const;
    void clear();

private:
    std::vector<Value> values_;
};

} // namespace mantra
