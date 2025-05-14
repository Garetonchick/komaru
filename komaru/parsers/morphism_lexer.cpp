#include "morphism_lexer.hpp"

#include <komaru/util/std_extensions.hpp>

#include <format>

namespace komaru::parsers {

std::string ToString(MorphismTokenType type) {
    switch (type) {
        case MorphismTokenType::Identifier:
            return "Identifier";
        case MorphismTokenType::Operator:
            return "Operator";
        case MorphismTokenType::IntLiteral:
            return "IntLiteral";
        case MorphismTokenType::RealLiteral:
            return "RealLiteral";
        case MorphismTokenType::StringLiteral:
            return "StringLiteral";
        case MorphismTokenType::CharLiteral:
            return "CharLiteral";
        case MorphismTokenType::LParen:
            return "LParen";
        case MorphismTokenType::RParen:
            return "RParen";
        case MorphismTokenType::LBracket:
            return "LBracket";
        case MorphismTokenType::RBracket:
            return "RBracket";
        case MorphismTokenType::Comma:
            return "Comma";
        case MorphismTokenType::End:
            return "End";
    }
}

std::string ToString(const MorphismToken& token) {
    return std::format("{}: \"{}\"", ToString(token.type), token.raw);
}

namespace {

bool CanStartIdentifier(char c) {
    return std::isalpha(c) || c == '_';
}

bool CanBeInIdentifier(char c) {
    return std::isalnum(c) || c == '\'' || c == '.' || c == '_';
}

bool CanBeInOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '=' || c == '>' ||
           c == '<' || c == '&' || c == '|' || c == '^' || c == '~' || c == '.' || c == '$' ||
           c == '!';
}

}  // namespace

ParserResult<std::vector<MorphismToken>> TokenizeMorphism(const std::string& raw) {
    std::vector<MorphismToken> tokens;

    auto add_number_token = [&](size_t& i) -> std::optional<ParserError> {
        size_t end = i;
        bool is_real = false;

        if (end == '-') {
            ++end;
        }
        while (end < raw.size() && std::isdigit(raw[end])) {
            ++end;
        }
        if (raw[end] == '.') {
            ++end;
            is_real = true;
        }
        while (end < raw.size() && std::isdigit(raw[end])) {
            ++end;
        }

        if (end == i) {
            return ParserError("failed to tokenize number");
        }

        std::string snum = raw.substr(i, end - i);

        if (is_real) {
            auto maybe_real = util::ReadReal(snum);
            if (!maybe_real) {
                return ParserError("failed to tokenize real number");
            }

            tokens.push_back(
                {.type = MorphismTokenType::RealLiteral, .raw = snum, .val = maybe_real.value()});
        } else {
            auto maybe_int = util::ReadInteger(snum);
            if (!maybe_int) {
                return ParserError("failed to tokenize integer");
            }

            tokens.push_back(
                {.type = MorphismTokenType::IntLiteral, .raw = snum, .val = maybe_int.value()});
        }

        i = end;
        return std::nullopt;
    };

    auto add_identifier_token = [&](size_t& i) -> std::optional<ParserError> {
        size_t end = i;
        if (!CanStartIdentifier(raw[end])) {
            return ParserError("failed to tokenize an identifier");
        }

        while (end < raw.size() && CanBeInIdentifier(raw[end])) {
            ++end;
        }

        tokens.push_back({.type = MorphismTokenType::Identifier,
                          .raw = raw.substr(i, end - i),
                          .val = std::monostate{}});
        i = end;
        return std::nullopt;
    };

    auto add_operator_token = [&](size_t& i) -> std::optional<ParserError> {
        size_t end = i;
        if (!CanBeInOperator(raw[end])) {
            return ParserError(std::format("failed to tokenize an operator, char {}", raw[end]));
        }
        while (end < raw.size() && CanBeInOperator(raw[end])) {
            ++end;
        }

        tokens.push_back({
            .type = MorphismTokenType::Operator,
            .raw = raw.substr(i, end - i),
            .val = std::monostate{},
        });
        i = end;
        return std::nullopt;
    };

    for (size_t i = 0; i < raw.size();) {
        if (std::isspace(raw[i])) {
            ++i;
            continue;
        }

        size_t was_i = i;

        switch (raw[i]) {
            case '(':
                tokens.push_back({
                    .type = MorphismTokenType::LParen,
                    .raw = "(",
                    .val = std::monostate{},
                });
                ++i;
                break;
            case ')':
                tokens.push_back({
                    .type = MorphismTokenType::RParen,
                    .raw = ")",
                    .val = std::monostate{},
                });
                ++i;
                break;
            case '[':
                tokens.push_back(
                    {.type = MorphismTokenType::LBracket, .raw = "[", .val = std::monostate{}});
                ++i;
                break;
            case ']':
                tokens.push_back({
                    .type = MorphismTokenType::RBracket,
                    .raw = "]",
                    .val = std::monostate{},
                });
                ++i;
                break;
            case ',':
                tokens.push_back({
                    .type = MorphismTokenType::Comma,
                    .raw = ",",
                    .val = std::monostate{},
                });
                ++i;
                break;
            default:
        }

        if (i != was_i) {
            continue;
        }

        if (raw[i] == '\'') {
            if (i + 2 >= raw.size() || raw[i + 2] != '\'') {
                return MakeParserError("error while tokenizing char literal");
            }

            tokens.push_back({.type = MorphismTokenType::CharLiteral,
                              .raw = std::string(1, raw[i + 1]),
                              .val = raw[i + 1]});
            i += 3;
            continue;
        }

        if (raw[i] == '\"') {
            size_t end_pos = std::find(raw.begin() + i + 1, raw.end(), '\"') - raw.begin();

            if (end_pos == raw.size()) {
                return MakeParserError("double quotes mismatch");
            }

            tokens.push_back({.type = MorphismTokenType::StringLiteral,
                              .raw = raw.substr(i, end_pos - i + 1),
                              .val = raw.substr(i + 1, end_pos - i - 1)});
            i = end_pos + 1;
            continue;
        }

        if (std::isdigit(raw[i]) ||
            (raw[i] == '-' && i + 1 < raw.size() && std::isdigit(raw[i + 1]))) {
            auto maybe_err = add_number_token(i);
            if (maybe_err) {
                return std::unexpected(maybe_err.value());
            }
            continue;
        }

        if (CanStartIdentifier(raw[i])) {
            auto maybe_err = add_identifier_token(i);
            if (maybe_err) {
                return std::unexpected(maybe_err.value());
            }
            continue;
        }

        auto maybe_err = add_operator_token(i);
        if (maybe_err) {
            return std::unexpected(maybe_err.value());
        }
    }

    tokens.push_back({
        .type = MorphismTokenType::End,
        .raw = "",
        .val = std::monostate{},
    });

    return tokens;
}

}  // namespace komaru::parsers
