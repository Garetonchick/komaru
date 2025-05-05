#pragma once

#include <komaru/translate/symbols_registry_like.hpp>

namespace komaru::translate {

class SimpleSymbolsRegistry {
public:
    SimpleSymbolsRegistry();

    std::optional<SymbolInfo> FindSymbol(const std::string& name) const;

private:
    std::unordered_map<std::string, SymbolInfo> symbol2info_;
};

static_assert(SymbolsRegistryLike<SimpleSymbolsRegistry>);

}  // namespace komaru::translate
