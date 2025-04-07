#include "value.hpp"

namespace komaru::lang {

Type AtomValue::GetType() const {
    return type_;
}

const AtomValue::Variant* AtomValue::GetVariantPointer() const {
    return &value_;
}

TupleValue::TupleValue(std::vector<Value> values)
    : type_(DetermineType(values))
    , values_(std::move(values)) {
}

Type TupleValue::GetType() const {
    return type_;
}

const std::vector<Value>& TupleValue::GetValues() const {
    return values_;
}

Type TupleValue::DetermineType(const std::vector<Value>& values) {
    std::vector<Type> types;

    for(const Value& value : values) {
        types.push_back(value.GetType());
    }

    return Type::Tuple(std::move(types));
}

Value Value::Tuple(std::vector<Value> values) {
    return TupleValue(std::move(values));
}

Value Value::Int(int32_t value) {
    return AtomValue(value);
}

Value Value::Bool(bool value) {
    return AtomValue(value);
}

Value Value::Char(char value) {
    return AtomValue(value);
}

Type Value::GetType() const {
    return Visit([](const auto& value) {
        return value.GetType();
    });
}

const Value::Variant* Value::GetVariantPointer() const {
    return &value_;
}


}
