#include "pattern.hpp"

namespace komaru::lang {

ValuePattern::ValuePattern(Value value)
    : value_(std::move(value)) {
}

const Value& ValuePattern::GetValue() const {
    return value_;
}

TuplePattern::TuplePattern(std::vector<Pattern> patterns)
    : patterns_(std::move(patterns)) {
}

const std::vector<Pattern>& TuplePattern::GetPatterns() const {
    return patterns_;
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

// For CRTP
const Pattern::Variant* Pattern::GetVariantPointer() const {
    return &pattern_;
}

}  // namespace komaru::lang
