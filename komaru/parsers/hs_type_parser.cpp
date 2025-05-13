#include <komaru/parsers/hs_type_parser.hpp>

#include <format>

namespace komaru::parsers {

HsTypeParser::HsTypeParser(const std::string& raw) {
    tokens_ = TokenizeHsType(raw).value();
}

ParserResult<lang::Type> HsTypeParser::Parse() {
    try {
        lang::Type type = ParseType();
        Expect(HsTypeTokenType::End);
        return type;
    } catch (const std::exception& e) {
        return MakeParserError(e.what());
    }
}

const HsTypeToken& HsTypeParser::Peek() const {
    return tokens_[cur_pos_];
}

const HsTypeToken& HsTypeParser::Consume() {
    if (cur_pos_ >= tokens_.size()) {
        throw std::runtime_error("consume out of bounds");
    }
    return tokens_[cur_pos_++];
}

const HsTypeToken& HsTypeParser::Expect(HsTypeTokenType type) {
    if (Peek().type != type) {
        throw std::runtime_error(
            std::format("expected token type {}, got {}", ToString(type), ToString(Peek().type)));
    }
    return Consume();
}

lang::Type HsTypeParser::ParseType(bool stop_on_arrow) {
    lang::Type src_type;

    if (Peek().type == HsTypeTokenType::Identifier) {
        src_type = ParseParameterizedType();
    } else if (Peek().type == HsTypeTokenType::LBracket) {
        Consume();
        src_type = lang::Type::List(ParseType());
        Expect(HsTypeTokenType::RBracket);
    } else if (Peek().type == HsTypeTokenType::LParen) {
        src_type = ParseTupleType();
    } else {
        throw std::runtime_error(std::format("unexpected token {}", ToString(Peek())));
    }

    if (stop_on_arrow) {
        return src_type;
    }

    if (Peek().type == HsTypeTokenType::Arrow) {
        Consume();
        auto dst_type = ParseType();
        return lang::Type::Function(src_type, dst_type);
    }
    return src_type;
}

lang::Type HsTypeParser::ParseParameterizedType() {
    std::string name = Expect(HsTypeTokenType::Identifier).raw;

    if (name == "String") {
        name = "Str";
    }

    std::vector<lang::Type> params;
    while (true) {
        if (Peek().type == HsTypeTokenType::Identifier) {
            params.push_back(lang::Type::Common(Consume().raw));
        } else if (CanBeTypeBegining()) {
            auto type = ParseType(true);
            params.push_back(type);
        } else {
            break;
        }
    }

    return lang::Type::Parameterized(std::move(name), std::move(params));
}

lang::Type HsTypeParser::ParseTupleType() {
    Expect(HsTypeTokenType::LParen);
    std::vector<lang::Type> types;

    while (true) {
        if (CanBeTypeBegining()) {
            types.push_back(ParseType());
        } else {
            break;
        }

        if (Peek().type == HsTypeTokenType::RParen) {
            break;
        }

        Expect(HsTypeTokenType::Comma);
    }

    Expect(HsTypeTokenType::RParen);

    if (types.empty()) {
        return lang::Type::Singleton();
    }
    if (types.size() == 1) {
        return types[0];
    }
    return lang::Type::Tuple(std::move(types));
}

bool HsTypeParser::CanBeTypeBegining() const {
    switch (Peek().type) {
        case HsTypeTokenType::Identifier:
            return true;
        case HsTypeTokenType::LParen:
            return true;
        case HsTypeTokenType::LBracket:
            return true;
        default:
            return false;
    }
}

}  // namespace komaru::parsers
