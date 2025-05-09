#pragma once

#include <komaru/lang/type.hpp>
#include <komaru/translate/haskell/hs_expr.hpp>

#include <string>

namespace komaru::translate::hs {

class HaskellDefinition {
public:
    static HaskellDefinition Normal(std::string name, std::vector<std::string> param_names,
                                    lang::Type type, HaskellExpr expr);
    static HaskellDefinition Unpack(std::string name, std::vector<std::string> unpack_names,
                                    lang::Type type);

    std::string ToString() const;
    const std::string& GetName() const;
    const std::vector<std::string>& GetParamNames() const;
    const lang::Type& GetType() const;
    const HaskellExpr& GetExpr() const;
    bool IsNormal() const;
    bool IsUnpack() const;

    void ChangeName(std::string new_name);

private:
    HaskellDefinition(std::string name, std::vector<std::string> param_names, lang::Type type,
                      std::optional<HaskellExpr> expr);

    std::string ToStringNormal() const;
    std::string ToStringUnpack() const;

private:
    std::string name_;
    std::vector<std::string> param_names_;
    lang::Type type_;
    std::optional<HaskellExpr> expr_;
};

}  // namespace komaru::translate::hs
