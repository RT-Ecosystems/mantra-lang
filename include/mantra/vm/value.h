#pragma once

#include <cstddef>
#include <string>
#include <variant>

namespace mantra {

enum class VMValueType {
    Number,
    String,
    Boolean,
    Null
};

class Value {
public:
    static Value number(double value);
    static Value string(std::string value);
    static Value boolean(bool value);
    static Value null();

    VMValueType type() const;
    bool isNumber() const;
    bool isString() const;
    bool isBoolean() const;
    bool isNull() const;

    double asNumber() const;
    const std::string& asString() const;
    bool asBoolean() const;

    std::string toString() const;

private:
    using Storage = std::variant<double, std::string, bool, std::monostate>;

    explicit Value(Storage value);

    Storage value_;
};

bool operator==(const Value& left, const Value& right);
bool operator!=(const Value& left, const Value& right);

} // namespace mantra
