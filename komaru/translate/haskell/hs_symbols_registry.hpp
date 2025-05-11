#pragma once

#include <komaru/translate/symbols_registry_like.hpp>
#include <komaru/translate/haskell/hs_import.hpp>
#include <komaru/translate/haskell/ghci.hpp>

namespace komaru::translate::hs {

class HaskellSymbolsRegistry {
public:
    HaskellSymbolsRegistry(std::vector<std::string> packages, std::vector<HaskellImport> imports);

    void SetPackages(std::vector<std::string> packages);
    void SetImports(std::vector<HaskellImport> imports);
    void Reload();
    void AddLocalSymbol(const std::string& name, SymbolInfo info);
    void ResetLocalSymbols();

    std::optional<SymbolInfo> FindSymbol(const std::string& name) const;

private:
    std::unique_ptr<GHCI> ghci_;
    std::vector<std::string> packages_;
    std::vector<HaskellImport> imports_;
    std::unordered_map<std::string, SymbolInfo> local_symbols_;
    mutable std::unordered_map<std::string, SymbolInfo> global_symbols_cache_;
};

static_assert(SymbolsRegistryLike<HaskellSymbolsRegistry>);

}  // namespace komaru::translate::hs
