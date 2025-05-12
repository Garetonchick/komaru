#pragma once

#include <komaru/lang/morphism.hpp>
#include <komaru/parsers/morphism_lexer.hpp>
#include <komaru/translate/haskell/hs_symbols_registry.hpp>

namespace komaru::parsers {

class MorphismParser {
public:
    explicit MorphismParser(const std::string& raw,
                            translate::hs::HaskellSymbolsRegistry& symbols_registry);

    ParserResult<lang::MorphismPtr> Parse();

private:
    const MorphismToken& Peek() const;
    const MorphismToken& Consume();
    const MorphismToken& Expect(MorphismTokenType type);

    lang::MorphismPtr ParseExprInCurrentScope();
    lang::MorphismPtr ParseSubExpr();
    lang::MorphismPtr ParseTupleExpr();
    lang::MorphismPtr ParseListExpr();
    lang::MorphismPtr ParseIdentifier();
    lang::MorphismPtr ParseOperator();

private:
    translate::hs::HaskellSymbolsRegistry& symbols_registry_;
    std::string raw_;
    std::vector<MorphismToken> tokens_;
    size_t cur_pos_{0};
};

}  // namespace komaru::parsers
