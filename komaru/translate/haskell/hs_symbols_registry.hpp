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
    void AddLocalFunction(const std::string& name, lang::Type type);
    void AddGlobalFunction(const std::string& name, lang::Type type);
    void ResetLocalFunctions();
    void ResetGlobalFunctions();
    std::optional<lang::Type> FindFunction(const std::string& name) const;
    std::optional<lang::TypeConstructor> FindTypeConstructor(const std::string& name) const;

private:
    std::unique_ptr<GHCI> ghci_;
    std::vector<std::string> packages_;
    std::vector<HaskellImport> imports_;
    std::unordered_map<std::string, lang::Type> local_symbols_;
    std::unordered_map<std::string, lang::Type> global_symbols_;
    mutable std::unordered_map<std::string, lang::Type> lib_symbols_cache_;
};

}  // namespace komaru::translate::hs
