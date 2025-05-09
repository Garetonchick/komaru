#include "hs_definition.hpp"

#include <sstream>
#include <format>

#include <komaru/util/std_extensions.hpp>
#include <komaru/util/string.hpp>
#include <komaru/lang/style.hpp>

namespace komaru::translate::hs {

HaskellDefinition::HaskellDefinition(std::string name, std::vector<std::string> param_names,
                                     lang::Type type, std::optional<HaskellExpr> expr)
    : name_(std::move(name)),
      param_names_(std::move(param_names)),
      type_(std::move(type)),
      expr_(std::move(expr)) {
}

std::string HaskellDefinition::ToString() const {
    if (IsNormal()) {
        return ToStringNormal();
    } else {
        return ToStringUnpack();
    }
}

const std::string& HaskellDefinition::GetName() const {
    return name_;
}

const std::vector<std::string>& HaskellDefinition::GetParamNames() const {
    return param_names_;
}

const lang::Type& HaskellDefinition::GetType() const {
    return type_;
}

const HaskellExpr& HaskellDefinition::GetExpr() const {
    return expr_.value();
}

void HaskellDefinition::ChangeName(std::string new_name) {
    name_ = std::move(new_name);
}

bool HaskellDefinition::IsNormal() const {
    return expr_.has_value();
}

bool HaskellDefinition::IsUnpack() const {
    return !expr_.has_value();
}

HaskellDefinition HaskellDefinition::Normal(std::string name, std::vector<std::string> param_names,
                                            lang::Type type, HaskellExpr expr) {
    return HaskellDefinition(std::move(name), std::move(param_names), std::move(type),
                             std::move(expr));
}

HaskellDefinition HaskellDefinition::Unpack(std::string name, std::vector<std::string> unpack_names,
                                            lang::Type type) {
    return HaskellDefinition(std::move(name), std::move(unpack_names), std::move(type),
                             std::nullopt);
}

std::string HaskellDefinition::ToStringNormal() const {
    std::stringstream ss;
    if (type_ != lang::Type::Auto()) {
        ss << std::format("{} :: {}\n", name_, type_.ToString(lang::Style::Haskell));
    }
    ss << name_;

    for (const auto& param_name : param_names_) {
        ss << " " << param_name;
    }

    ss << " =\n";
    ss << util::Indent(expr_.value().ToString(), util::k2S);

    return ss.str();
}

std::string HaskellDefinition::ToStringUnpack() const {
    std::stringstream ss;
    ss << "(";
    for (auto [i, param_name] : util::Enumerate(param_names_)) {
        ss << param_name;
        if (i + 1 < param_names_.size()) {
            ss << ", ";
        }
    }
    ss << ") = " << name_;

    return ss.str();
}

}  // namespace komaru::translate::hs
