#pragma once

#include <string>
#include <vector>

#include <komaru/parsers/parser_result.hpp>

namespace komaru::parsers {

enum class HsTypeTokenType {
    Identifier,
    LParen,
    RParen,
    LBracket,
    RBracket,
    Arrow,
    Comma,
    End,
};

std::string ToString(HsTypeTokenType type);

struct HsTypeToken {
    HsTypeTokenType type;
    std::string raw;
};

std::string ToString(const HsTypeToken& token);
ParserResult<std::vector<HsTypeToken>> TokenizeHsType(const std::string& raw);

}  // namespace komaru::parsers
