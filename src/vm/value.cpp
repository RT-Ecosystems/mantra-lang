#include "mantra/vm/value.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace mantra {

Value::Value(Storage value)
    : value_(std::move(value)) {}

Value Value::number(double value) {
    return Value(Storage{value});
}

Value Value::string(std::string value) {
    return Value(Storage{std::move(value)});
}

Value Value::boolean(bool value) {
    return Value(Storage{value});
}

Value Value::null() {
    return Value(Storage{std::monostate{}});
}

VMValueType Value::type() const {
    if (isNumber()) {
        return VMValueType::Number;
    }
    if (isString()) {
        return VMValueType::String;
    }
    if (isBoolean()) {
        return VMValueType::Boolean;
    }
    return VMValueType::Null;
}

bool Value::isNumber() const {
    return std::holds_alternative<double>(value_);
}

bool Value::isString() const {
    return std::holds_alternative<std::string>(value_);
}

bool Value::isBoolean() const {
    return std::holds_alternative<bool>(value_);
}

bool Value::isNull() const {
    return std::holds_alternative<std::monostate>(value_);
}

double Value::asNumber() const {
    if (!isNumber()) {
        throw std::runtime_error("Value is not a number");
    }
    return std::get<double>(value_);
}

const std::string& Value::asString() const {
    if (!isString()) {
        throw std::runtime_error("Value is not a string");
    }
    return std::get<std::string>(value_);
}

bool Value::asBoolean() const {
    if (!isBoolean()) {
        throw std::runtime_error("Value is not a boolean");
    }
    return std::get<bool>(value_);
}

std::string Value::toString() const {
    if (isNumber()) {
        std::ostringstream out;
        out << std::get<double>(value_);
        return out.str();
    }
    if (isString()) {
        return std::get<std::string>(value_);
    }
    if (isBoolean()) {
        return std::get<bool>(value_) ? "true" : "false";
    }
    return "null";
}

bool operator==(const Value& left, const Value& right) {
    if (left.type() != right.type()) {
        return false;
    }

    switch (left.type()) {
        case VMValueType::Number:
            return left.asNumber() == right.asNumber();
        case VMValueType::String:
            return left.asString() == right.asString();
        case VMValueType::Boolean:
            return left.asBoolean() == right.asBoolean();
        case VMValueType::Null:
            return true;
    }

    return false;
}

bool operator!=(const Value& left, const Value& right) {
    return !(left == right);
}

} // namespace mantra
