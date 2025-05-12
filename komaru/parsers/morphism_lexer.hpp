#pragma once

#include <komaru/parsers/parser_result.hpp>

#include <variant>
#include <vector>

namespace komaru::parsers {

enum class MorphismTokenType {
    Identifier,
    Operator,
    IntLiteral,
    RealLiteral,
    StringLiteral,
    CharLiteral,
    LParen,
    RParen,
    LBracket,
    RBracket,
    Comma,
    End,
};

std::string ToString(MorphismTokenType type);

using MorphismTokenValue = std::variant<std::monostate, int64_t, double, std::string, char>;

struct MorphismToken {
    MorphismTokenType type;
    std::string raw;
    MorphismTokenValue val;
};

std::string ToString(const MorphismToken& token);

ParserResult<std::vector<MorphismToken>> TokenizeMorphism(const std::string& raw);

}  // namespace komaru::parsers
