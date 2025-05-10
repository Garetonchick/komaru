#pragma once

#include <komaru/util/derive_variant.hpp>
#include <komaru/lang/morphism.hpp>
#include <komaru/lang/style.hpp>

namespace komaru::lang {

class Pattern;

struct AnyPattern;
class LiteralPattern;
class NamePattern;
class ConstructorPattern;
class TuplePattern;
// class ListPattern;
// class ConsPattern;

// dummy pattern indicating the '*'
struct AnyPattern {
    std::string ToString(Style style = Style::Komaru) const;
};

class LiteralPattern {
public:
    explicit LiteralPattern(Literal literal);

    const Literal& GetLiteral() const;
    std::string ToString(Style style = Style::Komaru) const;

private:
    Literal literal_;
};

class NamePattern {
public:
    explicit NamePattern(std::string name);

    const std::string& GetName() const;
    std::string ToString(Style style = Style::Komaru) const;

private:
    std::string name_;
};

class ConstructorPattern {
public:
    explicit ConstructorPattern(std::string name, std::vector<Pattern> patterns);

    const std::string& GetName() const;
    const std::vector<Pattern>& GetPatterns() const;
    std::string ToString(Style style = Style::Komaru) const;

private:
    std::string name_;
    std::vector<Pattern> patterns_;
};

class TuplePattern {
public:
    explicit TuplePattern(std::vector<Pattern> patterns);

    const std::vector<Pattern>& GetPatterns() const;
    std::string ToString(Style style = Style::Komaru) const;

private:
    std::vector<Pattern> patterns_;
};

class Pattern : public util::DeriveVariant<Pattern> {
    using Variant =
        std::variant<AnyPattern, LiteralPattern, NamePattern, ConstructorPattern, TuplePattern>;

public:
    static Pattern FromLiteral(Literal literal);
    static Pattern FromName(std::string name);
    static Pattern Constructor(std::string name, std::vector<Pattern> patterns);
    static Pattern Tuple(std::vector<Pattern> patterns);
    static Pattern Any();

    static Pattern Number(int64_t num);
    static Pattern Real(double real);
    static Pattern True();
    static Pattern False();
    static Pattern Char(char ch);
    static Pattern String(std::string str);

    std::string ToString(Style style = Style::Komaru) const;

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
