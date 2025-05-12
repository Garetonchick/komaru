#pragma once

#include <komaru/lang/type.hpp>
#include <komaru/parsers/cat_type_lexer.hpp>
#include <komaru/translate/haskell/hs_symbols_registry.hpp>

namespace komaru::parsers {

class CatTypeParser {
public:
    CatTypeParser(const std::string& raw, translate::hs::HaskellSymbolsRegistry& symbols_registry);

    ParserResult<lang::Type> Parse();

private:
    const CatTypeToken& Peek() const;
    const CatTypeToken& Consume();
    const CatTypeToken& Expect(CatTypeTokenType type);

    lang::Type ParseTuple();
    lang::Type ParseSubType();
    lang::Type ParseIdentifier();

    void ExpectTypeHasNParams(const std::string& raw, size_t n);

private:
    translate::hs::HaskellSymbolsRegistry& symbols_registry_;
    std::string raw_;
    std::vector<CatTypeToken> tokens_;
    size_t cur_pos_{0};
};

}  // namespace komaru::parsers
