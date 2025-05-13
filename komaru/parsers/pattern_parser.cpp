#include "pattern_parser.hpp"

#include <komaru/util/string.hpp>

#include <format>

namespace komaru::parsers {

PatternParser::PatternParser(const std::string& raw,
                             translate::hs::HaskellSymbolsRegistry& symbols_registry)
    : symbols_registry_(symbols_registry),
      raw_(raw) {
}

ParserResult<lang::Pattern> PatternParser::Parse() {
    auto maybe_tokens = TokenizePattern(raw_);
    if (!maybe_tokens) {
        return std::unexpected(maybe_tokens.error());
    }

    tokens_ = std::move(maybe_tokens.value());

    try {
        auto pattern = ParsePattern();
        Expect(PatternTokenType::End);
        return pattern;
    } catch (std::exception& e) {
        return MakeParserError(
            std::format("error while parsing \"{}\" morphism: {}", raw_, e.what()));
    }
}

const PatternToken& PatternParser::Peek() const {
    return tokens_[cur_pos_];
}

const PatternToken& PatternParser::Consume() {
    if (cur_pos_ >= tokens_.size()) {
        throw std::runtime_error("consume out of bounds");
    }
    return tokens_[cur_pos_++];
}

const PatternToken& PatternParser::Expect(PatternTokenType type) {
    if (Peek().type != type) {
        throw std::runtime_error(
            std::format("expected token type {}, got {}", ToString(type), ToString(Peek().type)));
    }
    return Consume();
}

namespace {

bool CanStartPattern(PatternTokenType type) {
    return type == PatternTokenType::Any || type == PatternTokenType::Identifier ||
           type == PatternTokenType::LParen || type == PatternTokenType::LBracket ||
           type == PatternTokenType::CharLiteral || type == PatternTokenType::IntLiteral ||
           type == PatternTokenType::RealLiteral || type == PatternTokenType::StringLiteral;
}

}  // namespace

lang::Pattern PatternParser::ParsePattern() {
    switch (Peek().type) {
        case PatternTokenType::Identifier:
            return ParseIdentifier();
        case PatternTokenType::LParen:
            return ParseTuple();
        case PatternTokenType::LBracket:
            return ParseList();
        case PatternTokenType::Any:
            Consume();
            return lang::Pattern::Any();
        case PatternTokenType::IntLiteral:
            return lang::Pattern::FromLiteral(
                lang::Literal::Number(std::get<int64_t>(Consume().val)));
        case PatternTokenType::RealLiteral:
            return lang::Pattern::FromLiteral(lang::Literal::Real(std::get<double>(Consume().val)));
        case PatternTokenType::CharLiteral:
            return lang::Pattern::FromLiteral(lang::Literal::Char(std::get<char>(Consume().val)));
        case PatternTokenType::StringLiteral:
            return lang::Pattern::FromLiteral(
                lang::Literal::String(std::get<std::string>(Consume().val)));
        default:
            break;
    }

    throw std::runtime_error(std::format("unexpected token type {}", ToString(Peek().type)));
}

lang::Pattern PatternParser::ParseIdentifier() {
    if (lang::IsConstructorName(Peek().raw)) {
        return ParseConstructor();
    }
    return lang::Pattern::FromName(Expect(PatternTokenType::Identifier).raw);
}

lang::Pattern PatternParser::ParseConstructor() {
    auto& tok = Expect(PatternTokenType::Identifier);
    size_t n = NumConstructorParams(tok.raw);

    std::vector<lang::Pattern> patterns;

    while (CanStartPattern(Peek().type)) {
        patterns.push_back(ParsePattern());
    }

    if (patterns.size() != n) {
        throw std::runtime_error("wrong number of parameters for constructor");
    }

    return lang::Pattern::Constructor(tok.raw, std::move(patterns));
}

lang::Pattern PatternParser::ParseTuple() {
    Expect(PatternTokenType::LParen);

    std::vector<lang::Pattern> patterns;

    while (Peek().type != PatternTokenType::RParen) {
        patterns.emplace_back(ParsePattern());
        if (Peek().type != PatternTokenType::RParen) {
            Expect(PatternTokenType::Comma);
        }
    }

    Expect(PatternTokenType::RParen);

    return lang::Pattern::Tuple(std::move(patterns));
}

lang::Pattern PatternParser::ParseList() {
    throw std::runtime_error("not implemented");
}

size_t PatternParser::NumConstructorParams(const std::string& name) const {
    auto maybe_type = symbols_registry_.FindFunction(name);
    if (!maybe_type) {
        throw std::runtime_error(std::format("constructor \"{}\" not found", name));
    }

    auto type = maybe_type.value();

    return type.GetParamNum();
}

}  // namespace komaru::parsers
