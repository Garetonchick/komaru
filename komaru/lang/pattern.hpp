#pragma once

#include <komaru/util/derive_variant.hpp>
#include <komaru/lang/value.hpp>

namespace komaru::lang {

class Pattern;

// dummy pattern indicating the '_'
struct AnyPattern {
    std::string ToString() const;
};

class ValuePattern {
public:
    explicit ValuePattern(Value value);

    const Value& GetValue() const;
    std::string ToString() const;

private:
    Value value_;
};

class TuplePattern {
public:
    explicit TuplePattern(std::vector<Pattern> patterns);

    const std::vector<Pattern>& GetPatterns() const;
    std::string ToString() const;

private:
    std::vector<Pattern> patterns_;
};

class Pattern : public util::DeriveVariant<Pattern> {
    using Variant = std::variant<AnyPattern, ValuePattern, TuplePattern>;

public:
    static Pattern FromValue(Value value);
    static Pattern TupleFromPatterns(std::vector<Pattern> patterns);
    static Pattern Any();

    std::string ToString() const;

    // For CRTP
    const Variant* GetVariantPointer() const;

private:
    template <typename T>
    explicit Pattern(T pattern)
        : pattern_(std::move(pattern)) {
    }

private:
    Variant pattern_;
};

}  // namespace komaru::lang
