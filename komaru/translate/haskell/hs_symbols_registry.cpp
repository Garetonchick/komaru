#include "hs_symbols_registry.hpp"

namespace komaru::translate::hs {

HaskellSymbolsRegistry::HaskellSymbolsRegistry(std::vector<std::string> packages,
                                               std::vector<HaskellImport> imports)
    : packages_(std::move(packages)),
      imports_(std::move(imports)) {
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
    global_symbols_cache_.clear();
}

void HaskellSymbolsRegistry::AddLocalSymbol(const std::string& name, SymbolInfo info) {
    local_symbols_.emplace(name, std::move(info));
}

void HaskellSymbolsRegistry::ResetLocalSymbols() {
    local_symbols_.clear();
}

std::optional<SymbolInfo> HaskellSymbolsRegistry::FindSymbol(const std::string& name) const {
    auto it = local_symbols_.find(name);
    if (it != local_symbols_.end()) {
        return it->second;
    }
    if (global_symbols_cache_.find(name) != global_symbols_cache_.end()) {
        return it->second;
    }

    auto maybe_type = ghci_->GetType(name);
    if (maybe_type.has_value()) {
        global_symbols_cache_[name] = maybe_type.value();
        return maybe_type.value();
    }

    auto maybe_constructor = ghci_->GetTypeConstructor(name);
    if (!maybe_constructor) {
        return std::nullopt;
    }

    global_symbols_cache_.emplace(name, maybe_constructor.value());
    return maybe_constructor.value();
}

}  // namespace komaru::translate::hs
