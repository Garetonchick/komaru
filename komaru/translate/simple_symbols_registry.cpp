#include "simple_symbols_registry.hpp"

namespace komaru::translate {

SimpleSymbolsRegistry::SimpleSymbolsRegistry() {
    symbol2info_ = {
        {"Int", SymbolInfo{.type = lang::Type::Int(), .kind = SymbolKind::Type}},
        {"Bool", SymbolInfo{.type = lang::Type::Bool(), .kind = SymbolKind::Type}},
        {"Char", SymbolInfo{.type = lang::Type::Char(), .kind = SymbolKind::Type}},
        {"Float", SymbolInfo{.type = lang::Type::Float(), .kind = SymbolKind::Type}},
        {"Int x Int", SymbolInfo{.type = lang::Type::Int().Pow(2), .kind = SymbolKind::Type}},
        {"S", SymbolInfo{.type = lang::Type::Singleton(), .kind = SymbolKind::Type}},
        {"IO S", SymbolInfo{.type = lang::Type::Parameterized("IO", {lang::Type::Singleton()}),
                            .kind = SymbolKind::Type}},
        {"IO Int", SymbolInfo{.type = lang::Type::Parameterized("IO", {lang::Type::Int()}),
                              .kind = SymbolKind::Type}},
        {"IO Int x IO Int",
         SymbolInfo{.type = lang::Type::Parameterized("IO", {lang::Type::Int()}).Pow(2),
                    .kind = SymbolKind::Type}}};

    auto at = lang::Type::Generic("a");
    auto bt = lang::Type::Generic("b");
    auto ct = lang::Type::Generic("c");
    auto io_a = lang::Type::Parameterized("IO", {at});
    auto io_b = lang::Type::Parameterized("IO", {bt});
    auto io_c = lang::Type::Parameterized("IO", {ct});
    auto io_s = lang::Type::Parameterized("IO", {lang::Type::Singleton()});

    symbol2info_.emplace("read",
                         SymbolInfo{.type = lang::Type::Function(
                                        lang::Type::Singleton(),
                                        lang::Type::Parameterized("IO", {lang::Type::Auto()})),
                                    .kind = SymbolKind::Function});
    symbol2info_.emplace("liftM2",
                         SymbolInfo{.type = lang::Type::Function(
                                        lang::Type::Function(at * bt, ct) * io_a * io_b, io_c),
                                    .kind = SymbolKind::Function});
    symbol2info_.emplace(
        ">>=", SymbolInfo{.type = lang::Type::Function(io_a * lang::Type::Function(at, io_b), io_b),
                          .kind = SymbolKind::Function});
    symbol2info_.emplace("print", SymbolInfo{.type = lang::Type::Function(lang::Type::Auto(), io_s),
                                             .kind = SymbolKind::Function});
}

std::optional<SymbolInfo> SimpleSymbolsRegistry::FindSymbol(const std::string& name) const {
    auto it = symbol2info_.find(name);
    if (it == symbol2info_.end()) {
        return std::nullopt;
    }

    return it->second;
}

}  // namespace komaru::translate
