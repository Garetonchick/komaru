#pragma once

#include <util/derive_variant.hpp>
#include <lang/value.hpp>

namespace komaru::lang {

class Pattern;

struct AnyPattern {}; // dummy pattern indicating the '_'

class ValuePattern {
public:
    explicit ValuePattern(Value value);

    const Value& GetValue() const;

private:
    Value value_;
};

class TuplePattern {
public:
    explicit TuplePattern(std::vector<Pattern> patterns);

    const std::vector<Pattern>& GetPatterns() const;

private:
    std::vector<Pattern> patterns_;
};

class Pattern : public util::DeriveVariant<Pattern> {
    using Variant = std::variant<AnyPattern, ValuePattern, TuplePattern>;
public:
    static Pattern FromValue(Value value);
    static Pattern TupleFromPatterns(std::vector<Pattern> patterns);
    static Pattern Any();

    // For CRTP
    const Variant* GetVariantPointer() const;

private:
    template<typename T>
    Pattern(T pattern) : pattern_(std::move(pattern)) {}

private:
    Variant pattern_;
};

}
