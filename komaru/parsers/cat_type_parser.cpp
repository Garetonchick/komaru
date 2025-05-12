#include "cat_type_parser.hpp"

#include <format>

namespace komaru::parsers {

CatTypeParser::CatTypeParser(const std::string& raw,
                             translate::hs::HaskellSymbolsRegistry& symbols_registry)
    : symbols_registry_(symbols_registry),
      raw_(raw) {
}

static bool CanStartSubType(CatTypeTokenType type) {
    return type == CatTypeTokenType::Identifier || type == CatTypeTokenType::LParen ||
           type == CatTypeTokenType::LBracket;
}

ParserResult<lang::Type> CatTypeParser::Parse() {
    auto maybe_tokens = TokenizeCatType(raw_);
    if (!maybe_tokens) {
        return std::unexpected(maybe_tokens.error());
    }

    tokens_ = std::move(maybe_tokens.value());

    try {
        auto morphism = ParseTuple();
        Expect(CatTypeTokenType::End);
        return morphism;
    } catch (std::exception& e) {
        return MakeParserError(e.what());
    }
}

const CatTypeToken& CatTypeParser::Peek() const {
    return tokens_[cur_pos_];
}

const CatTypeToken& CatTypeParser::Consume() {
    if (cur_pos_ >= tokens_.size()) {
        throw std::runtime_error("consume out of bounds");
    }
    return tokens_[cur_pos_++];
}

const CatTypeToken& CatTypeParser::Expect(CatTypeTokenType type) {
    if (Peek().type != type) {
        throw std::runtime_error(
            std::format("expected token type {}, got {}", ToString(type), ToString(Peek().type)));
    }
    return Consume();
}

lang::Type CatTypeParser::ParseTuple() {
    if (Peek().type == CatTypeTokenType::LParen) {
        Consume();
        auto type = ParseTuple();
        Expect(CatTypeTokenType::RParen);
        return type;
    }

    std::vector<lang::Type> types;

    while (true) {
        types.push_back(ParseSubType());
        if (Peek().type == CatTypeTokenType::X) {
            Consume();
        } else {
            break;
        }
    }

    if (types.size() == 1) {
        return types[0];
    }
    return lang::Type::Tuple(std::move(types));
}

lang::Type CatTypeParser::ParseSubType() {
    if (Peek().type == CatTypeTokenType::LParen) {
        Consume();
        auto type = ParseTuple();
        Expect(CatTypeTokenType::RParen);
        return type;
    }
    if (Peek().type == CatTypeTokenType::LBracket) {
        Consume();
        auto type = ParseTuple();
        Expect(CatTypeTokenType::RBracket);
        return lang::Type::List(type);
    }
    if (Peek().type == CatTypeTokenType::Identifier) {
        auto type = ParseIdentifier();
        return type;
    }

    throw std::runtime_error("unexpected token type");
}

lang::Type CatTypeParser::ParseIdentifier() {
    auto& tok = Expect(CatTypeTokenType::Identifier);
    std::vector<lang::Type> params;

    while (CanStartSubType(Peek().type)) {
        if (Peek().type == CatTypeTokenType::Identifier) {
            ExpectTypeHasNParams(Peek().raw, 0);
            params.push_back(lang::Type::Simple(Peek().raw));
            Consume();
        } else {
            params.push_back(ParseSubType());
        }
    }

    ExpectTypeHasNParams(tok.raw, params.size());

    return lang::Type::Parameterized(tok.raw, std::move(params));
}

void CatTypeParser::ExpectTypeHasNParams(const std::string& raw, size_t n) {
    if (!lang::IsConcreteTypeName(raw)) {
        return;
    }
    auto maybe_type_constructor = symbols_registry_.FindTypeConstructor(raw);
    if (!maybe_type_constructor) {
        throw std::runtime_error("unknown type");
    }

    auto& type_constructor = maybe_type_constructor.value();

    if (type_constructor.GetNumParams() != n) {
        throw std::runtime_error("wrong number of parameters for type constructor");
    }
}

}  // namespace komaru::parsers
