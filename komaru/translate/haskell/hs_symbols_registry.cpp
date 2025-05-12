#include "hs_symbols_registry.hpp"

#include <print>

namespace komaru::translate::hs {

HaskellSymbolsRegistry::HaskellSymbolsRegistry(std::vector<std::string> packages,
                                               std::vector<HaskellImport> imports)
    : packages_(std::move(packages)),
      imports_(std::move(imports)) {
    Reload();
}

void HaskellSymbolsRegistry::SetPackages(std::vector<std::string> packages) {
    packages_ = std::move(packages);
}

void HaskellSymbolsRegistry::SetImports(std::vector<HaskellImport> imports) {
    imports_ = std::move(imports);
}

void HaskellSymbolsRegistry::Reload() {
    auto maybe_ghci = GHCI::CreateSession(packages_, imports_);
    if (!maybe_ghci) {
        throw std::runtime_error("failed to create ghci session");
    }
    ghci_ = std::move(maybe_ghci.value());
    lib_symbols_cache_.clear();
}

void HaskellSymbolsRegistry::AddLocalFunction(const std::string& name, lang::Type type) {
    local_symbols_.emplace(name, type);
}

void HaskellSymbolsRegistry::AddGlobalFunction(const std::string& name, lang::Type type) {
    global_symbols_.emplace(name, type);
}

void HaskellSymbolsRegistry::ResetLocalFunctions() {
    local_symbols_.clear();
}

void HaskellSymbolsRegistry::ResetGlobalFunctions() {
    global_symbols_.clear();
}

std::optional<lang::Type> HaskellSymbolsRegistry::FindFunction(const std::string& name) const {
    auto it = local_symbols_.find(name);
    if (it != local_symbols_.end()) {
        return it->second;
    }

    it = global_symbols_.find(name);
    if (it != global_symbols_.end()) {
        return it->second;
    }

    it = lib_symbols_cache_.find(name);
    if (it != lib_symbols_cache_.end()) {
        return it->second;
    }

    auto maybe_type = ghci_->GetType(name);
    if (maybe_type.has_value()) {
        lib_symbols_cache_[name] = maybe_type.value();
        return maybe_type.value();
    }

    return std::nullopt;
}

std::optional<lang::TypeConstructor> HaskellSymbolsRegistry::FindTypeConstructor(
    const std::string& name) const {
    if (name == "S") {
        return lang::TypeConstructor("S", 0);
    }

    return ghci_->GetTypeConstructor(name);
}

}  // namespace komaru::translate::hs
