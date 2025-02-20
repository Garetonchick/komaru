#pragma once
#include <lang/type.hpp>
#include <util/derive_variant.hpp>

#include <cstdint>
#include <variant>

namespace komaru::lang {

class Value;

class AtomValue : public util::DeriveVariant<AtomValue> {
    using Variant = std::variant<int32_t, bool, char>;
public:
    template<typename T>
    explicit AtomValue(T value) : value_(value), type_(DetermineType<T>()) {}

    Type GetType() const;

    // For CRTP
    const Variant* GetVariantPointer() const;

private:
    template<typename T>
    static Type DetermineType() {
        if constexpr (std::is_same_v<T, int32_t>) {
            return Type::FromTag(TypeTag::Int);
        } else if constexpr (std::is_same_v<T, bool>) {
            return Type::FromTag(TypeTag::Bool);
        } else if constexpr (std::is_same_v<T, char>) {
            return Type::FromTag(TypeTag::Char);
        } else {
            static_assert(false, "Wrong type");
        }
    }

    Variant value_;
    Type type_;
};

class TupleValue {
public:
    explicit TupleValue(std::vector<Value> values);

    Type GetType() const;
    const std::vector<Value>& GetValues() const;

private:
    static Type DetermineType(const std::vector<Value>& values);

private:
    Type type_;
    std::vector<Value> values_;
};

class Value : public util::DeriveVariant<Value> {
    using Variant = std::variant<AtomValue, TupleValue>;
public:
    static Value NewInt(int32_t value);
    static Value NewBool(bool value);
    static Value NewChar(char value);

    static Value NewTuple(std::vector<Value> values);

    template<typename T>
    static Value NewAtom(T value) {
        return Value(AtomValue(value));
    }

    template<typename... Args>
    static Value NewTupleRaw(Args&&... args) {
        return NewTuple({Value::NewAtom(args)...});
    }

    Type GetType() const;

    // For CRTP
    const Variant* GetVariantPointer() const;

private:
    template<typename T>
    Value(T value) : value_(std::move(value)) {}

private:
    Variant value_;
};

}
