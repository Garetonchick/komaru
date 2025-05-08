#include "pattern.hpp"

#include <komaru/util/std_extensions.hpp>

namespace komaru::lang {

std::string AnyPattern::ToString() const {
    return "*";
}

LiteralPattern::LiteralPattern(Literal literal)
    : literal_(std::move(literal)) {
}

const Literal& LiteralPattern::GetLiteral() const {
    return literal_;
}

std::string LiteralPattern::ToString() const {
    return literal_.ToString();
}

NamePattern::NamePattern(std::string name)
    : name_(std::move(name)) {
}

const std::string& NamePattern::GetName() const {
    return name_;
}

std::string NamePattern::ToString() const {
    return name_;
}

ConstructorPattern::ConstructorPattern(std::string name, std::vector<Pattern> patterns)
    : name_(std::move(name)),
      patterns_(std::move(patterns)) {
}

const std::string& ConstructorPattern::GetName() const {
    return name_;
}

const std::vector<Pattern>& ConstructorPattern::GetPatterns() const {
    return patterns_;
}

std::string ConstructorPattern::ToString() const {
    throw std::runtime_error("Not implemented");
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

Pattern Pattern::FromLiteral(Literal literal) {
    return Pattern(LiteralPattern(std::move(literal)));
}

Pattern Pattern::FromName(std::string name) {
    return Pattern(NamePattern(std::move(name)));
}

Pattern Pattern::Constructor(std::string name, std::vector<Pattern> patterns) {
    return Pattern(ConstructorPattern(std::move(name), std::move(patterns)));
}

Pattern Pattern::Tuple(std::vector<Pattern> patterns) {
    return Pattern(TuplePattern(std::move(patterns)));
}

Pattern Pattern::Any() {
    return Pattern(AnyPattern{});
}

Pattern Pattern::Number(int64_t num) {
    return FromLiteral(Literal::Number(num));
}

Pattern Pattern::Real(double real) {
    return FromLiteral(Literal::Real(real));
}

Pattern Pattern::True() {
    return Constructor("True", {});
}

Pattern Pattern::False() {
    return Constructor("False", {});
}

Pattern Pattern::Char(char ch) {
    return FromLiteral(Literal::Char(ch));
}

Pattern Pattern::String(std::string str) {
    return FromLiteral(Literal::String(std::move(str)));
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
