#include "hs_type_lexer.hpp"

#include <format>

namespace komaru::parsers {

bool CanBeIdentifier(char c) {
    return std::isalnum(c) || c == '.';
}

ParserResult<std::vector<HsTypeToken>> TokenizeHsType(const std::string& raw) {
    std::vector<HsTypeToken> tokens;

    for (size_t i = 0; i < raw.size();) {
        if (std::isspace(raw[i])) {
            ++i;
            continue;
        }

        size_t was_i = i;

        switch (raw[i]) {
            case '(':
                tokens.push_back({HsTypeTokenType::LParen, "("});
                ++i;
                break;
            case ')':
                tokens.push_back({HsTypeTokenType::RParen, ")"});
                ++i;
                break;
            case '[':
                tokens.push_back({HsTypeTokenType::LBracket, "["});
                ++i;
                break;
            case ']':
                tokens.push_back({HsTypeTokenType::RBracket, "]"});
                ++i;
                break;
            case ',':
                tokens.push_back({HsTypeTokenType::Comma, ","});
                ++i;
                break;
            case '-':
                if (i + 1 >= raw.size()) {
                    return MakeParserError(std::format("expected arrow head but got nothing"));
                }
                if (raw[i + 1] == '>') {
                    tokens.push_back({HsTypeTokenType::Arrow, "->"});
                    i += 2;
                    break;
                }

                return MakeParserError(
                    std::format("expected arrow head but got: \'{}\'", raw[i + 1]));
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

        tokens.push_back({HsTypeTokenType::Identifier, raw.substr(start, i - start)});
    }

    tokens.push_back({.type = HsTypeTokenType::End, .raw = ""});
    return tokens;
}

std::string ToString(HsTypeTokenType type) {
    switch (type) {
        case HsTypeTokenType::Identifier:
            return "Identifier";
        case HsTypeTokenType::LParen:
            return "LParen";
        case HsTypeTokenType::RParen:
            return "RParen";
        case HsTypeTokenType::LBracket:
            return "LBracket";
        case HsTypeTokenType::RBracket:
            return "RBracket";
        case HsTypeTokenType::Arrow:
            return "Arrow";
        case HsTypeTokenType::Comma:
            return "Comma";
        case HsTypeTokenType::End:
            return "End";
    }
}

std::string ToString(const HsTypeToken& token) {
    return std::format("{}: {}", ToString(token.type), token.raw);
}

}  // namespace komaru::parsers
