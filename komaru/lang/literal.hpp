#pragma once
#include <komaru/lang/type.hpp>
#include <komaru/util/derive_variant.hpp>

#include <cstdint>
#include <variant>

namespace komaru::lang {

class Literal;

class Literal : public util::DeriveVariant<Literal> {
    using Variant = std::variant<int64_t, double, char, std::string>;

public:
    static Literal Number(int32_t value);
    static Literal Real(double value);
    static Literal Char(char value);
    static Literal String(std::string value);

    template <typename T>
    static Literal Make(T value) {
        return Literal(std::move(value));
    }

    Type GetType() const;
    std::string ToString() const;

    // For CRTP
    const Variant* GetVariantPointer() const;

private:
    template <typename T>
    explicit Literal(T value)
        : value_(std::move(value)) {
    }

private:
    Variant value_;
};

}  // namespace komaru::lang
