#include "pattern.hpp"

#include <komaru/util/std_extensions.hpp>

namespace komaru::lang {

std::string AnyPattern::ToString() const {
    return "*";
}

ValuePattern::ValuePattern(Value value)
    : value_(std::move(value)) {
}

const Value& ValuePattern::GetValue() const {
    return value_;
}

std::string ValuePattern::ToString() const {
    return value_.ToString();
}

TuplePattern::TuplePattern(std::vector<Pattern> patterns)
    : patterns_(std::move(patterns)) {
}

const std::vector<Pattern>& TuplePattern::GetPatterns() const {
    return patterns_;
}

std::string TuplePattern::ToString() const {
    std::string s = "(";

    for (const auto& [i, pattern] : util::Enumerate(patterns_)) {
        s += pattern.ToString();
        if (i + 1 != patterns_.size()) {
            s += ", ";
        }
    }

    s += ")";
    return s;
}

Pattern Pattern::FromValue(Value value) {
    return Pattern(ValuePattern(std::move(value)));
}

Pattern Pattern::TupleFromPatterns(std::vector<Pattern> patterns) {
    return Pattern(TuplePattern(std::move(patterns)));
}

Pattern Pattern::Any() {
    return Pattern(AnyPattern{});
}

std::string Pattern::ToString() const {
    return Visit([](const auto& pattern) {
        return pattern.ToString();
    });
}

// For CRTP
const Pattern::Variant* Pattern::GetVariantPointer() const {
    return &pattern_;
}

}  // namespace komaru::lang
