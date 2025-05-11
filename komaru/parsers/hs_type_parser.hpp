#pragma once

#include <komaru/lang/type.hpp>
#include <komaru/parsers/hs_type_lexer.hpp>

namespace komaru::parsers {

class HsTypeParser {
public:
    explicit HsTypeParser(const std::string& raw);

    ParserResult<lang::Type> Parse();

private:
    const HsTypeToken& Peek() const;
    const HsTypeToken& Consume();
    const HsTypeToken& Expect(HsTypeTokenType type);

    lang::Type ParseType(bool stop_on_arrow = false);
    lang::Type ParseParameterizedType();
    lang::Type ParseTupleType();

    bool CanBeTypeBegining() const;

private:
    std::vector<HsTypeToken> tokens_;
    size_t cur_pos_{0};
};

}  // namespace komaru::parsers
