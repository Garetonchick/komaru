#pragma once

#include <komaru/parsers/parser_result.hpp>

#include <variant>
#include <vector>

namespace komaru::parsers {

enum class PatternTokenType {
    Identifier,
    Any,
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

std::string ToString(PatternTokenType type);

using PatternTokenValue = std::variant<std::monostate, int64_t, double, std::string, char>;

struct PatternToken {
    PatternTokenType type;
    std::string raw;
    PatternTokenValue val;
};

std::string ToString(const PatternToken& token);

ParserResult<std::vector<PatternToken>> TokenizePattern(const std::string& raw);

}  // namespace komaru::parsers
