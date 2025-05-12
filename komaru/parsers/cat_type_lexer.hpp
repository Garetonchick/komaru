#pragma once

#include <string>
#include <vector>

#include <komaru/parsers/parser_result.hpp>

namespace komaru::parsers {

enum class CatTypeTokenType {
    Identifier,
    LParen,
    RParen,
    LBracket,
    RBracket,
    X,
    End,
};

std::string ToString(CatTypeTokenType type);

struct CatTypeToken {
    CatTypeTokenType type;
    std::string raw;
};

std::string ToString(const CatTypeToken& token);
ParserResult<std::vector<CatTypeToken>> TokenizeCatType(const std::string& raw);

}  // namespace komaru::parsers
