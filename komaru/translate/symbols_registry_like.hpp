#pragma once

#include <komaru/lang/type.hpp>

#include <concepts>
#include <string>
#include <optional>

namespace komaru::translate {

using SymbolInfo = std::variant<lang::Type, lang::TypeConstructor>;

template <typename T>
concept SymbolsRegistryLike = requires(const T reg, std::string s) {
    { reg.FindSymbol(s) } -> std::convertible_to<std::optional<SymbolInfo>>;
};

}  // namespace komaru::translate
