#include "mantra/vm/stack.h"

#include <stdexcept>

namespace mantra {

void Stack::push(const Value& value) {
    values_.push_back(value);
}

Value Stack::pop() {
    if (values_.empty()) {
        throw std::runtime_error("Stack underflow");
    }

    Value value = values_.back();
    values_.pop_back();
    return value;
}

const Value& Stack::peek(std::size_t distance) const {
    if (distance >= values_.size()) {
        throw std::runtime_error("Stack underflow");
    }

    return values_[values_.size() - 1 - distance];
}

std::size_t Stack::size() const {
    return values_.size();
}

bool Stack::empty() const {
    return values_.empty();
}

void Stack::clear() {
    values_.clear();
}

} // namespace mantra
