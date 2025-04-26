#include "value.hpp"

#include <komaru/util/std_extensions.hpp>

namespace komaru::lang {

Type AtomValue::GetType() const {
    return type_;
}

std::string AtomValue::ToString() const {
    return std::visit(util::Overloaded{[](bool val) -> std::string {
                                           return val ? "True" : "False";
                                       },
                                       [](char val) -> std::string {
                                           return "\'" + std::string(1, val) + "\'";
                                       },
                                       [](int32_t val) -> std::string {
                                           return std::to_string(val);
                                       }},
                      value_);
}

const AtomValue::Variant* AtomValue::GetVariantPointer() const {
    return &value_;
}

TupleValue::TupleValue(std::vector<Value> values)
    : type_(DetermineType(values)),
      values_(std::move(values)) {
}

Type TupleValue::GetType() const {
    return type_;
}

const std::vector<Value>& TupleValue::GetValues() const {
    return values_;
}

std::string TupleValue::ToString() const {
    std::string res = "(";

    for (const auto& [i, value] : util::Enumerate(values_)) {
        res += value.ToString();
        if (i + 1 != values_.size()) {
            res += ", ";
        }
    }

    res += ")";
    return res;
}

Type TupleValue::DetermineType(const std::vector<Value>& values) {
    std::vector<Type> types;

    for (const Value& value : values) {
        types.push_back(value.GetType());
    }

    return Type::Tuple(std::move(types));
}

Value Value::Tuple(std::vector<Value> values) {
    return Value(TupleValue(std::move(values)));
}

Value Value::Int(int32_t value) {
    return Value(AtomValue(value));
}

Value Value::Bool(bool value) {
    return Value(AtomValue(value));
}

Value Value::Char(char value) {
    return Value(AtomValue(value));
}

Type Value::GetType() const {
    return Visit([](const auto& value) {
        return value.GetType();
    });
}

std::string Value::ToString() const {
    return Visit([](const auto& value) {
        return value.ToString();
    });
}

const Value::Variant* Value::GetVariantPointer() const {
    return &value_;
}

}  // namespace komaru::lang
