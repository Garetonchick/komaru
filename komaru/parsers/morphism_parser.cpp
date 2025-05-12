#include "morphism_parser.hpp"

#include <format>
#include <print>

namespace komaru::parsers {

MorphismParser::MorphismParser(const std::string& raw,
                               translate::hs::HaskellSymbolsRegistry& symbols_registry)
    : symbols_registry_(symbols_registry),
      raw_(raw) {
}

ParserResult<lang::MorphismPtr> MorphismParser::Parse() {
    auto maybe_tokens = TokenizeMorphism(raw_);
    if (!maybe_tokens) {
        return std::unexpected(maybe_tokens.error());
    }

    tokens_ = std::move(maybe_tokens.value());

    try {
        auto morphism = ParseExprInCurrentScope();
        Expect(MorphismTokenType::End);
        return morphism;
    } catch (std::exception& e) {
        return MakeParserError(
            std::format("error while parsing \"{}\" morphism: {}", raw_, e.what()));
    }
}

const MorphismToken& MorphismParser::Peek() const {
    return tokens_[cur_pos_];
}

const MorphismToken& MorphismParser::Consume() {
    if (cur_pos_ >= tokens_.size()) {
        throw std::runtime_error("consume out of bounds");
    }
    return tokens_[cur_pos_++];
}

const MorphismToken& MorphismParser::Expect(MorphismTokenType type) {
    if (Peek().type != type) {
        throw std::runtime_error(
            std::format("expected token type {}, got {}", ToString(type), ToString(Peek().type)));
    }
    return Consume();
}

static bool CanStartSubExpr(MorphismTokenType type) {
    return type == MorphismTokenType::LParen || type == MorphismTokenType::LBracket ||
           type == MorphismTokenType::Identifier || type == MorphismTokenType::IntLiteral ||
           type == MorphismTokenType::RealLiteral || type == MorphismTokenType::CharLiteral ||
           type == MorphismTokenType::StringLiteral;
}

lang::MorphismPtr MorphismParser::ParseExprInCurrentScope() {
    if (Peek().type == MorphismTokenType::Operator && Peek().raw == "$") {
        Consume();
        if (Peek().type == MorphismTokenType::End) {
            return lang::Morphism::NonePosition();
        }
        auto& tok = Expect(MorphismTokenType::IntLiteral);
        int64_t val = std::get<int64_t>(tok.val);
        if (val < 0) {
            throw std::runtime_error("negative position for position morphism");
        }
        return lang::Morphism::Position(val);
    }

    if (Peek().type == MorphismTokenType::Operator) {
        auto op = ParseOperator();
        if (!CanStartSubExpr(Peek().type)) {
            return op;
        }
        auto end_expr = ParseExprInCurrentScope();
        return lang::Morphism::Binded(op, {{1, end_expr}});
    }

    auto cur_expr = ParseSubExpr();

    while (CanStartSubExpr(Peek().type)) {
        if (cur_expr->GetParamNum() == 0) {
            throw std::runtime_error("too many arguments to a function call");
        }
        auto nxt_expr = ParseSubExpr();
        cur_expr = lang::Morphism::Binded(cur_expr, {{0, nxt_expr}});
    }

    if (Peek().type == MorphismTokenType::Operator) {
        auto op = ParseOperator();
        auto end_expr = ParseExprInCurrentScope();
        return lang::Morphism::Binded(op, {{0, cur_expr}, {1, end_expr}});
    }

    return cur_expr;
}

lang::MorphismPtr MorphismParser::ParseSubExpr() {
    switch (Peek().type) {
        case MorphismTokenType::LParen:
            return ParseTupleExpr();
        case MorphismTokenType::LBracket:
            return ParseListExpr();
        case MorphismTokenType::Identifier:
            return ParseIdentifier();
        case MorphismTokenType::Operator:
            return ParseOperator();
        case MorphismTokenType::CharLiteral:
            return lang::Morphism::Literal(lang::Literal::Char(std::get<char>(Consume().val)));
        case MorphismTokenType::StringLiteral:
            return lang::Morphism::Literal(
                lang::Literal::String(std::get<std::string>(Consume().val)));
        case MorphismTokenType::IntLiteral:
            return lang::Morphism::Literal(lang::Literal::Number(std::get<int64_t>(Consume().val)));
        case MorphismTokenType::RealLiteral:
            return lang::Morphism::Literal(lang::Literal::Real(std::get<double>(Consume().val)));
        default:
            break;
    }

    throw std::runtime_error(
        std::format("failed to parse subexpr while parsing \"{}\" morphism", raw_));
}

lang::MorphismPtr MorphismParser::ParseTupleExpr() {
    Expect(MorphismTokenType::LParen);
    std::vector<lang::MorphismPtr> exprs;

    while (true) {
        auto expr = ParseExprInCurrentScope();
        exprs.push_back(std::move(expr));
        if (Peek().type != MorphismTokenType::Comma) {
            break;
        }
        Consume();
    }

    Expect(MorphismTokenType::RParen);

    if (exprs.empty()) {
        return lang::Morphism::Singleton();
    }

    if (exprs.size() == 1) {
        return exprs[0];
    }

    return lang::Morphism::Tuple(std::move(exprs));
}

lang::MorphismPtr MorphismParser::ParseListExpr() {
    throw std::runtime_error("not implemented");
}

lang::MorphismPtr MorphismParser::ParseIdentifier() {
    const auto& tok = Expect(MorphismTokenType::Identifier);
    auto maybe_type = symbols_registry_.FindFunction(tok.raw);

    if (!maybe_type) {
        throw std::runtime_error(std::format("unknown function identifier \"{}\"", tok.raw));
    }

    auto type = maybe_type.value();

    return lang::Morphism::CommonWithType(tok.raw, type);
}

lang::MorphismPtr MorphismParser::ParseOperator() {
    const auto& tok = Expect(MorphismTokenType::Operator);
    auto maybe_type = symbols_registry_.FindFunction("(" + tok.raw + ")");

    if (!maybe_type) {
        throw std::runtime_error("unknown operator");
    }

    auto type = maybe_type.value();

    return lang::Morphism::CommonWithType(tok.raw, type);
}

}  // namespace komaru::parsers
