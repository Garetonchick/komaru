#include "literal.hpp"

#include <komaru/util/std_extensions.hpp>

#include <format>

namespace komaru::lang {

Literal Literal::Number(int32_t value) {
    return Literal(value);
}

Literal Literal::Real(double value) {
    return Literal(value);
}

Literal Literal::Char(char value) {
    return Literal(value);
}

Literal Literal::String(std::string value) {
    return Literal(value);
}

Type Literal::GetType() const {
    return std::visit(util::Overloaded{[](int64_t) -> Type {
                                           return Type::Var("a");
                                       },
                                       [](double) -> Type {
                                           return Type::Var("a");
                                       },
                                       [](char) -> Type {
                                           return Type::Char();
                                       },
                                       [](std::string) -> Type {
                                           return Type::String();
                                       }},
                      value_);
}

std::string Literal::ToString() const {
    return std::visit(util::Overloaded{[](int64_t val) -> std::string {
                                           return std::to_string(val);
                                       },
                                       [](double val) -> std::string {
                                           return std::to_string(val);
                                       },
                                       [](char val) -> std::string {
                                           return std::format("\'{}\'", val);
                                       },
                                       [](std::string val) -> std::string {
                                           return std::format("\"{}\"", val);
                                       }},
                      value_);
}

const Literal::Variant* Literal::GetVariantPointer() const {
    return &value_;
}

}  // namespace komaru::lang
