#pragma once

#include <komaru/lang/type.hpp>

#include <string>

namespace komaru::translate::hs {

class HaskellExpr {
    using Variant = std::variant<std::string, std::vector<std::string>>;

public:
    static HaskellExpr Simple(lang::Type type, std::string expr);
    static HaskellExpr NamesTuple(lang::Type type, std::vector<std::string> names);

    bool IsSimple() const;
    bool IsNamesTuple() const;

    std::string ToString() const;
    const std::vector<std::string>& GetNames() const;
    std::vector<lang::Type> GetTypes() const;
    lang::Type GetType() const;

private:
    HaskellExpr(lang::Type type, Variant expr);

private:
    lang::Type type_;
    Variant expr_;
};

}  // namespace komaru::translate::hs
