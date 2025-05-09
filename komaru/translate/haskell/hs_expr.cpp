#include "hs_expr.hpp"

#include <komaru/util/std_extensions.hpp>

namespace komaru::translate::hs {

HaskellExpr HaskellExpr::Simple(lang::Type type, std::string expr) {
    return HaskellExpr(type, std::move(expr));
}

HaskellExpr HaskellExpr::NamesTuple(lang::Type type, std::vector<std::string> names) {
    if (type.GetComponentsNum() != names.size()) {
        throw std::invalid_argument("haskell expr: type and names size mismatch");
    }

    return HaskellExpr(type, std::move(names));
}

bool HaskellExpr::IsSimple() const {
    return std::holds_alternative<std::string>(expr_);
}

bool HaskellExpr::IsNamesTuple() const {
    return std::holds_alternative<std::vector<std::string>>(expr_);
}

std::string HaskellExpr::ToString() const {
    if (IsSimple()) {
        return std::get<std::string>(expr_);
    }

    const auto& names = std::get<std::vector<std::string>>(expr_);

    std::string res = "(";
    for (auto [i, name] : util::Enumerate(names)) {
        res += name;
        if (i + 1 < names.size()) {
            res += ", ";
        }
    }
    res += ")";
    return res;
}

const std::vector<std::string>& HaskellExpr::GetNames() const {
    return std::get<std::vector<std::string>>(expr_);
}

std::vector<lang::Type> HaskellExpr::GetTypes() const {
    return type_.Visit(util::Overloaded{
        [](const lang::TupleType& type) -> std::vector<lang::Type> {
            return type.GetTupleTypes();
        },
        [this](const auto&) -> std::vector<lang::Type> {
            return {type_};
        },

    });
}

lang::Type HaskellExpr::GetType() const {
    return type_;
}

HaskellExpr::HaskellExpr(lang::Type type, Variant expr)
    : type_(type),
      expr_(std::move(expr)) {
}

}  // namespace komaru::translate::hs
