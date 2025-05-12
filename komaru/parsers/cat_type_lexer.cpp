#include "cat_type_lexer.hpp"

#include <format>

namespace komaru::parsers {

std::string ToString(CatTypeTokenType type) {
    switch (type) {
        case CatTypeTokenType::Identifier:
            return "Identifier";
        case CatTypeTokenType::LParen:
            return "LParen";
        case CatTypeTokenType::RParen:
            return "RParen";
        case CatTypeTokenType::LBracket:
            return "LBracket";
        case CatTypeTokenType::RBracket:
            return "RBracket";
        case CatTypeTokenType::X:
            return "X";
        case CatTypeTokenType::End:
            return "End";
    }
}

std::string ToString(const CatTypeToken& token) {
    return std::format("{}: \"{}\"", ToString(token.type), token.raw);
}

namespace {

bool CanBeIdentifier(char c) {
    return std::isalnum(c) || c == '.';
}

}  // namespace

ParserResult<std::vector<CatTypeToken>> TokenizeCatType(const std::string& raw) {
    std::vector<CatTypeToken> tokens;

    for (size_t i = 0; i < raw.size();) {
        if (std::isspace(raw[i])) {
            ++i;
            continue;
        }

        size_t was_i = i;

        switch (raw[i]) {
            case '(':
                tokens.push_back({CatTypeTokenType::LParen, "("});
                ++i;
                break;
            case ')':
                tokens.push_back({CatTypeTokenType::RParen, ")"});
                ++i;
                break;
            case '[':
                tokens.push_back({CatTypeTokenType::LBracket, "["});
                ++i;
                break;
            case ']':
                tokens.push_back({CatTypeTokenType::RBracket, "]"});
                ++i;
                break;
            case 'x':
                tokens.push_back({CatTypeTokenType::X, "x"});
                ++i;
                break;
            default:
                break;
        }

        if (i != was_i) {
            continue;
        }

        if (!std::isalpha(raw[i])) {
            return MakeParserError(
                std::format("expected start of identifier but got: \'{}\'", raw[i]));
        }

        size_t start = i;
        while (i < raw.size() && CanBeIdentifier(raw[i])) {
            ++i;
        }

        tokens.push_back({CatTypeTokenType::Identifier, raw.substr(start, i - start)});
    }

    tokens.push_back({.type = CatTypeTokenType::End, .raw = ""});
    return tokens;
}

}  // namespace komaru::parsers
