#pragma once

#include <komaru/lang/pattern.hpp>
#include <komaru/parsers/pattern_lexer.hpp>
#include <komaru/translate/haskell/hs_symbols_registry.hpp>

namespace komaru::parsers {

class PatternParser {
public:
    PatternParser(const std::string& raw, translate::hs::HaskellSymbolsRegistry& symbols_registry);

    ParserResult<lang::Pattern> Parse();

private:
    const PatternToken& Peek() const;
    const PatternToken& Consume();
    const PatternToken& Expect(PatternTokenType type);

    lang::Pattern ParsePattern();
    lang::Pattern ParseIdentifier();
    lang::Pattern ParseConstructor();
    lang::Pattern ParseTuple();
    lang::Pattern ParseList();

    size_t NumConstructorParams(const std::string& name) const;

private:
    translate::hs::HaskellSymbolsRegistry& symbols_registry_;
    std::string raw_;
    std::vector<PatternToken> tokens_;
    size_t cur_pos_{0};
};

}  // namespace komaru::parsers
