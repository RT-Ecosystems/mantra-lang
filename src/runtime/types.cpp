#include "mantra/runtime/types.h"
#include <stdexcept>
#include <sstream>
#include <cmath>

namespace mantra {
namespace runtime {

// Value implementation

Value::Value() : type_(ValueType::Null) {}

Value Value::number(double value) {
    Value v(ValueType::Number);
    v.number_value_ = value;
    return v;
}

Value Value::string(std::string value) {
    Value v(ValueType::String);
    v.string_value_ = std::move(value);
    return v;
}

Value Value::boolean(bool value) {
    Value v(ValueType::Boolean);
    v.bool_value_ = value;
    return v;
}

Value Value::null() {
    return Value(ValueType::Null);
}

Value Value::function(const Function& fn) {
    Value v(ValueType::Function);
    v.function_value_ = std::make_shared<Function>(fn);
    return v;
}

Value Value::array(const std::vector<Value>& elements) {
    Value v(ValueType::Array);
    v.array_value_ = elements;
    return v;
}

Value Value::object(const std::unordered_map<std::string, Value>& properties) {
    Value v(ValueType::Object);
    v.object_value_ = std::make_shared<std::unordered_map<std::string, Value>>(properties);
    return v;
}

Value::Value(ValueType type) : type_(type) {
    if (type == ValueType::Object) {
        object_value_ = std::make_shared<std::unordered_map<std::string, Value>>();
    }
}

ValueType Value::type() const {
    return type_;
}

bool Value::isNumber() const {
    return type_ == ValueType::Number;
}

bool Value::isString() const {
    return type_ == ValueType::String;
}

bool Value::isBoolean() const {
    return type_ == ValueType::Boolean;
}

bool Value::isNull() const {
    return type_ == ValueType::Null;
}

bool Value::isFunction() const {
    return type_ == ValueType::Function;
}

bool Value::isArray() const {
    return type_ == ValueType::Array;
}

bool Value::isObject() const {
    return type_ == ValueType::Object;
}

double Value::asNumber() const {
    if (!isNumber()) {
        throw RuntimeException("Value is not a number");
    }
    return number_value_;
}

const std::string& Value::asString() const {
    if (!isString()) {
        throw RuntimeException("Value is not a string");
    }
    return string_value_;
}

bool Value::asBoolean() const {
    if (!isBoolean()) {
        throw RuntimeException("Value is not a boolean");
    }
    return bool_value_;
}

const Function& Value::asFunction() const {
    if (!isFunction()) {
        throw RuntimeException("Value is not a function");
    }
    return *function_value_;
}

const std::vector<Value>& Value::asArray() const {
    if (!isArray()) {
        throw RuntimeException("Value is not an array");
    }
    return array_value_;
}

const std::unordered_map<std::string, Value>& Value::asObject() const {
    if (!isObject()) {
        throw RuntimeException("Value is not an object");
    }
    return *object_value_;
}

std::string Value::toString() const {
    switch (type_) {
        case ValueType::Number: {
            // Format number without unnecessary decimals
            double intpart;
            if (modf(number_value_, &intpart) == 0.0) {
                return std::to_string(static_cast<long long>(intpart));
            }
            return std::to_string(number_value_);
        }
        case ValueType::String:
            return string_value_;
        case ValueType::Boolean:
            return bool_value_ ? "true" : "false";
        case ValueType::Null:
            return "null";
        case ValueType::Function:
            return "[Function: " + function_value_->name + "]";
        case ValueType::Array: {
            std::string result = "[";
            for (size_t i = 0; i < array_value_.size(); ++i) {
                if (i > 0) result += ", ";
                result += array_value_[i].toString();
            }
            result += "]";
            return result;
        }
        case ValueType::Object:
            return "[Object]";
        default:
            return "unknown";
    }
}

bool Value::toBoolean() const {
    switch (type_) {
        case ValueType::Boolean:
            return bool_value_;
        case ValueType::Null:
            return false;
        case ValueType::Number:
            return number_value_ != 0.0;
        case ValueType::String:
            return !string_value_.empty();
        default:
            return true;
    }
}

bool Value::equals(const Value& other) const {
    if (type_ != other.type_) {
        return false;
    }
    
    switch (type_) {
        case ValueType::Number:
            return number_value_ == other.number_value_;
        case ValueType::String:
            return string_value_ == other.string_value_;
        case ValueType::Boolean:
            return bool_value_ == other.bool_value_;
        case ValueType::Null:
            return true;
        case ValueType::Function:
            return function_value_.get() == other.function_value_.get();
        case ValueType::Array:
            return array_value_ == other.array_value_;
        case ValueType::Object:
            return *object_value_ == *other.object_value_;
        default:
            return false;
    }
}

// Environment implementation

Environment::Environment(std::shared_ptr<Environment> parent)
    : parent_(parent) {}

void Environment::define(const std::string& name, const Value& value) {
    values_[name] = value;
}

bool Environment::set(const std::string& name, const Value& value) {
    if (values_.find(name) != values_.end()) {
        values_[name] = value;
        return true;
    }
    
    if (parent_) {
        return parent_->set(name, value);
    }
    
    return false;
}

Value Environment::get(const std::string& name) const {
    auto it = values_.find(name);
    if (it != values_.end()) {
        return it->second;
    }
    
    if (parent_) {
        return parent_->get(name);
    }
    
    throw RuntimeException("Undefined variable: " + name);
}

bool Environment::exists(const std::string& name) const {
    if (values_.find(name) != values_.end()) {
        return true;
    }
    
    if (parent_) {
        return parent_->exists(name);
    }
    
    return false;
}

} // namespace runtime
} // namespace mantra
