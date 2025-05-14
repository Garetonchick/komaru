#include "pattern.hpp"

#include <komaru/util/std_extensions.hpp>

#include <cassert>

namespace komaru::lang {

static void MergeNameMappings(std::map<std::string, lang::Type>& a,
                              const std::map<std::string, lang::Type>& b) {
    for (auto [name, type] : b) {
        a[name] = type;
    }
}

std::string AnyPattern::ToString(Style style) const {
    switch (style) {
        case Style::Komaru:
            return "*";
        case Style::Haskell:
            return "_";
        case Style::Debug:
            return "Any";
    }
}

std::map<std::string, lang::Type> AnyPattern::GetNamesMapping(
    const translate::hs::HaskellSymbolsRegistry&, lang::Type) const {
    return {};
}

LiteralPattern::LiteralPattern(Literal literal)
    : literal_(std::move(literal)) {
}

const Literal& LiteralPattern::GetLiteral() const {
    return literal_;
}

std::string LiteralPattern::ToString(Style) const {
    return literal_.ToString();
}

std::map<std::string, lang::Type> LiteralPattern::GetNamesMapping(
    const translate::hs::HaskellSymbolsRegistry&, lang::Type) const {
    return {};
}

NamePattern::NamePattern(std::string name)
    : name_(std::move(name)) {
}

const std::string& NamePattern::GetName() const {
    return name_;
}

std::string NamePattern::ToString(Style) const {
    return name_;
}

std::map<std::string, lang::Type> NamePattern::GetNamesMapping(
    const translate::hs::HaskellSymbolsRegistry&, lang::Type type) const {
    return {{name_, type}};
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

std::string ConstructorPattern::ToString(Style style) const {
    std::string res = name_;

    for (const auto& pattern : patterns_) {
        res += " " + pattern.ToString(style);
    }

    return res;
}

std::map<std::string, lang::Type> ConstructorPattern::GetNamesMapping(
    const translate::hs::HaskellSymbolsRegistry& registry, lang::Type) const {
    auto maybe_cons_type = registry.FindFunction(name_);
    assert(maybe_cons_type.has_value());
    auto cons_type = maybe_cons_type.value();
    auto types = cons_type.FlattenFunction();
    assert(patterns_.size() + 1 == types.size());

    std::map<std::string, lang::Type> res;

    for (size_t i = 0; i < patterns_.size(); ++i) {
        MergeNameMappings(res, patterns_[i].GetNamesMapping(registry, types[i]));
    }
    return res;
}

TuplePattern::TuplePattern(std::vector<Pattern> patterns)
    : patterns_(std::move(patterns)) {
}

const std::vector<Pattern>& TuplePattern::GetPatterns() const {
    return patterns_;
}

std::string TuplePattern::ToString(Style style) const {
    std::string s = "(";

    for (const auto& [i, pattern] : util::Enumerate(patterns_)) {
        s += pattern.ToString(style);
        if (i + 1 != patterns_.size()) {
            s += ", ";
        }
    }

    s += ")";
    return s;
}

std::map<std::string, lang::Type> TuplePattern::GetNamesMapping(
    const translate::hs::HaskellSymbolsRegistry& registry, lang::Type type) const {
    assert(type.Holds<TupleType>());
    std::map<std::string, lang::Type> res;
    auto comps = type.GetComponents();
    assert(comps.size() == patterns_.size());

    for (size_t i = 0; i < patterns_.size(); ++i) {
        MergeNameMappings(res, patterns_[i].GetNamesMapping(registry, comps[i]));
    }

    return res;
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

std::string Pattern::ToString(Style style) const {
    return Visit([style](const auto& pattern) {
        return pattern.ToString(style);
    });
}

std::map<std::string, lang::Type> Pattern::GetNamesMapping(
    const translate::hs::HaskellSymbolsRegistry& registry, lang::Type type) const {
    return Visit([&](const auto& pattern) {
        return pattern.GetNamesMapping(registry, type);
    });
}

// For CRTP
const Pattern::Variant* Pattern::GetVariantPointer() const {
    return &pattern_;
}

}  // namespace komaru::lang
