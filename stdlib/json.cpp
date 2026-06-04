
#include "json.h"
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <sstream>

using json = nlohmann::json;

namespace mantra {
namespace stdlib {

MantraValue jsonToMantraValue(const json& j) {
    if (j.is_null()) return MantraValue::nullValue();
    if (j.is_boolean()) return MantraValue::boolean(j.get<bool>());
    if (j.is_number_integer()) return MantraValue::number(static_cast<double>(j.get<int64_t>()));
    if (j.is_number_float()) return MantraValue::number(j.get<double>());
    if (j.is_string()) return MantraValue::string(j.get<std::string>());
    if (j.is_array()) {
        MantraValue arr = MantraValue::array();
        for (const auto& item : j) arr.array_value.push_back(jsonToMantraValue(item));
        return arr;
    }
    if (j.is_object()) {
        MantraValue obj = MantraValue::object();
        for (auto& [key, value] : j.items()) obj.object_value[key] = jsonToMantraValue(value);
        return obj;
    }
    return MantraValue::nullValue();
}

json mantraValueToJson(const MantraValue& v) {
    switch (v.type) {
        case MantraValue::Type::NIL: return json(nullptr);
        case MantraValue::Type::BOOLEAN: return json(v.bool_value);
        case MantraValue::Type::NUMBER: return json(v.number_value);
        case MantraValue::Type::STRING: return json(v.string_value);
        case MantraValue::Type::ARRAY: {
            json arr = json::array();
            for (const auto& item : v.array_value) arr.push_back(mantraValueToJson(item));
            return arr;
        }
        case MantraValue::Type::OBJECT: {
            json obj = json::object();
            for (const auto& [key, value] : v.object_value) obj[key] = mantraValueToJson(value);
            return obj;
        }
        default: return json(nullptr);
    }
}

MantraValue builtinJsonParse(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING) throw std::runtime_error("json_parse: string required");
    try {
        json j = json::parse(args[0].string_value);
        return jsonToMantraValue(j);
    } catch (const json::exception& e) {
        throw std::runtime_error(std::string("JSON parse error: ") + e.what());
    }
}

MantraValue builtinJsonStringify(const std::vector<MantraValue>& args) {
    if (args.empty()) return MantraValue::string("null");
    try {
        json j = mantraValueToJson(args[0]);
        return MantraValue::string(j.dump(2));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON stringify error: ") + e.what());
    }
}

MantraValue builtinJsonStringifyCompact(const std::vector<MantraValue>& args) {
    if (args.empty()) return MantraValue::string("null");
    try {
        json j = mantraValueToJson(args[0]);
        return MantraValue::string(j.dump());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON stringify error: ") + e.what());
    }
}

MantraValue builtinJsonGet(const std::vector<MantraValue>& args) {
    if (args.size() < 2 || args[0].type != MantraValue::Type::OBJECT || args[1].type != MantraValue::Type::STRING) throw std::runtime_error("json_get: object and key string required");
    auto it = args[0].object_value.find(args[1].string_value);
    if (it != args[0].object_value.end()) return it->second;
    return MantraValue::nullValue();
}

MantraValue builtinJsonSet(const std::vector<MantraValue>& args) {
    if (args.size() < 3 || args[0].type != MantraValue::Type::OBJECT || args[1].type != MantraValue::Type::STRING) throw std::runtime_error("json_set: object, key, and value required");
    args[0].object_value[args[1].string_value] = args[2];
    return args[0];
}

MantraValue builtinJsonKeys(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::OBJECT) throw std::runtime_error("json_keys: object required");
    MantraValue arr = MantraValue::array();
    for (const auto& [key, _] : args[0].object_value) arr.array_value.push_back(MantraValue::string(key));
    return arr;
}

MantraValue builtinJsonType(const std::vector<MantraValue>& args) {
    if (args.empty() || args[0].type != MantraValue::Type::STRING) throw std::runtime_error("json_type: JSON string required");
    try {
        json j = json::parse(args[0].string_value);
        if (j.is_null()) return MantraValue::string("null");
        if (j.is_boolean()) return MantraValue::string("boolean");
        if (j.is_number()) return MantraValue::string("number");
        if (j.is_string()) return MantraValue::string("string");
        if (j.is_array()) return MantraValue::string("array");
        if (j.is_object()) return MantraValue::string("object");
        return MantraValue::string("unknown");
    } catch (...) { return MantraValue::string("invalid"); }
}

} // namespace stdlib
} // namespace mantra
