#include "cat_cooking.hpp"

#include <komaru/translate/simple_symbols_registry.hpp>
#include <komaru/util/string.hpp>
#include <komaru/util/std_extensions.hpp>

#include <format>
#include <cstdlib>
#include <print>

namespace komaru::translate {

using CookedNode = lang::CatProgram::Node;
using CookedArrow = lang::CatProgram::Arrow;
using CookedOutPin = lang::CatProgram::OutPin;
using CookedBrancher = CookedOutPin::Brancher;

template <SymbolsRegistryLike SymbolsRegistry>
class Cooker {
public:
    explicit Cooker(const SymbolsRegistry& symbols_registry)
        : symbols_registry_(symbols_registry) {
    }

    CookingResult<lang::CatProgram> Cook(const RawCatProgram& raw_program);

private:
    CookingResult<lang::MorphismPtr> CookMorphism(std::string raw_morphism);
    CookingResult<lang::MorphismPtr> CookSimpleMorphism(std::string raw_morphism);
    CookingResult<CookedBrancher> CookBrancher(const std::string& raw_brancher);
    CookingResult<lang::Pattern> CookPattern(std::string raw_pattern);
    CookingResult<lang::Guard> CookGuard(const std::string& raw_guard);
    CookingResult<lang::Literal> CookLiteral(std::string raw_literal);

    std::optional<SymbolInfo> FindSymbol(const std::string& symbol);

private:
    const SymbolsRegistry& symbols_registry_;
    std::unordered_map<std::string, SymbolInfo> global_symbols_;
};

// Assumes s is stripped
bool IsName(const std::string& s) {
    for (char c : s) {
        if (util::IsSpace(c)) {
            return false;
        }
    }
    return true;
}

CookingResult<int32_t> CookNumber(const char* s) {
    char* end = nullptr;
    errno = 0;
    int32_t num = std::strtol(s, &end, 10);

    if (errno || end != s + strlen(s)) {
        return MakeCookingError(std::format("\"{}\" is not a number", num));
    }

    return num;
}

template <SymbolsRegistryLike SymbolsRegistry>
std::optional<SymbolInfo> Cooker<SymbolsRegistry>::FindSymbol(const std::string& symbol) {
    auto it = global_symbols_.find(symbol);
    if (it != global_symbols_.end()) {
        return it->second;
    }

    return symbols_registry_.FindSymbol(symbol);
}

std::vector<lang::Type> CalcNode2Ret(const std::vector<CookedNode*>& cooked_nodes,
                                     const RawCatProgram& raw_program) {
    std::vector<lang::Type> rets(cooked_nodes.size(), lang::Type::Auto());
    std::vector<char> vis(cooked_nodes.size(), false);
    rets.reserve(cooked_nodes.size());
    std::vector<std::vector<size_t>> graph(cooked_nodes.size());

    for (const auto& conn : raw_program.GetConnections()) {
        graph[conn.source_node].push_back(conn.target_node);
    }

    auto dfs = [&](auto& self, size_t v) -> void {
        vis[v] = true;
        if (graph[v].empty()) {
            rets[v] = cooked_nodes[v]->GetType();
        }
        for (size_t u : graph[v]) {
            if (!vis[u]) {
                self(self, u);
            }
            rets[v] = rets[u];
            break;
        }
    };

    for (size_t v = 0; v < vis.size(); ++v) {
        if (!vis[v]) {
            dfs(dfs, v);
        }
    }

    return rets;
}

template <SymbolsRegistryLike SymbolsRegistry>
CookingResult<lang::CatProgram> Cooker<SymbolsRegistry>::Cook(const RawCatProgram& raw_program) {
    lang::CatProgramBuilder builder;
    std::vector<CookedNode*> id2cnode;
    std::vector<std::vector<CookedOutPin*>> pins;
    id2cnode.reserve(raw_program.GetNodes().size());
    pins.reserve(id2cnode.capacity());

    for (const auto& node : raw_program.GetNodes()) {
        auto maybe_sinfo = symbols_registry_.FindSymbol(node.type);

        if (!maybe_sinfo) {
            return MakeCookingError(std::format("type \"{}\" not found", node.type));
        }

        const auto& sinfo = maybe_sinfo.value();

        if (sinfo.kind != SymbolKind::Type) {
            return MakeCookingError(std::format("symbol \"{}\" is not a type", node.type));
        }

        auto& cnode = builder.NewNode(sinfo.type, node.name.value_or(""));
        id2cnode.push_back(&cnode);
        pins.push_back({});

        for (const auto& brancher : node.branchers) {
            auto maybe_cooked_brancher = CookBrancher(brancher);
            if (!maybe_cooked_brancher) {
                return std::unexpected(maybe_cooked_brancher.error());
            }

            std::visit(
                [&cnode, &pins](auto&& b) {
                    pins.back().push_back(&cnode.AddOutPin(b));
                },
                std::move(maybe_cooked_brancher.value()));
        }
    }

    auto rets = CalcNode2Ret(id2cnode, raw_program);

    for (const auto& [i, node] : util::Enumerate(raw_program.GetNodes())) {
        const auto* cnode = id2cnode[i];
        if (!node.has_input && node.name) {
            global_symbols_.emplace(node.name.value(),
                                    SymbolInfo{
                                        .type = lang::Type::Function(cnode->GetType(), rets[i]),
                                        .kind = SymbolKind::Function,
                                    });
        }
    }

    for (const auto& conn : raw_program.GetConnections()) {
        auto maybe_morphism = CookMorphism(conn.morphism);

        if (!maybe_morphism) {
            return std::unexpected(maybe_morphism.error());
        }
        auto* out_pin = pins[conn.source_node][conn.pin_idx];
        auto* target_node = id2cnode[conn.target_node];

        builder.Connect(*out_pin, *target_node, std::move(maybe_morphism.value()));
    }

    return builder.Extract();
}

// TODO: proper priority system for operators and also left/right associativity
template <SymbolsRegistryLike SymbolsRegistry>
CookingResult<lang::MorphismPtr> Cooker<SymbolsRegistry>::CookMorphism(std::string raw_morphism) {
    raw_morphism = util::Strip(raw_morphism);

    if (raw_morphism.empty()) {
        return MakeCookingError("empty morphisms are not allowed");
    }

    if (raw_morphism[0] == '!') {
        raw_morphism.erase(raw_morphism.begin());
        return CookMorphism(std::move(raw_morphism));
    }

    if (raw_morphism[0] == '$') {
        if (raw_morphism.size() == 1) {
            return lang::Morphism::NonePosition();
        }

        auto maybe_number = CookNumber(raw_morphism.c_str() + 1);

        if (!maybe_number) {
            return std::unexpected(maybe_number.error());
        }

        return lang::Morphism::Position(maybe_number.value());
    }

    if (raw_morphism[0] == '(') {
        auto maybe_literal = CookLiteral(raw_morphism);

        if (!maybe_literal) {
            return std::unexpected(maybe_literal.error());
        }

        return lang::Morphism::Literal(std::move(maybe_literal.value()));
    }

    if (IsName(raw_morphism)) {
        return CookSimpleMorphism(std::move(raw_morphism));
    }

    std::vector<lang::MorphismPtr> submorphisms;
    int64_t balance = 0;
    size_t last_start_idx = 0;
    bool last_space = false;

    auto mismatch_error =
        MakeCookingError(std::format("brackets mismatch in pattern \"{}\"", raw_morphism));

    for (size_t i = 0; i < raw_morphism.size(); ++i) {
        if (raw_morphism[i] == '(') {
            ++balance;
        } else if (raw_morphism[i] == ')') {
            --balance;
        }

        if (balance < 0) {
            return mismatch_error;
        }

        if (balance == 0 && !util::IsSpace(raw_morphism[i]) && last_space) {
            auto maybe_morphism =
                CookMorphism(raw_morphism.substr(last_start_idx, i - last_start_idx));

            if (!maybe_morphism) {
                return std::unexpected(std::move(maybe_morphism.error()));
            }

            submorphisms.emplace_back(std::move(maybe_morphism.value()));
            last_start_idx = i;
        }

        last_space = util::IsSpace(raw_morphism[i]);
    }

    if (balance != 0) {
        return mismatch_error;
    }

    auto maybe_morphism =
        CookMorphism(raw_morphism.substr(last_start_idx, raw_morphism.size() - last_start_idx));

    if (!maybe_morphism) {
        return std::unexpected(std::move(maybe_morphism.error()));
    }

    submorphisms.emplace_back(std::move(maybe_morphism.value()));

    if (submorphisms.size() > 2) {
        return MakeCookingError("TODO: complex morphisms are not supported");
    }

    if (submorphisms[0]->IsOperator()) {
        return lang::Morphism::Binded(submorphisms[0], {{1, submorphisms[1]}});
    }

    return lang::Morphism::Binded(submorphisms[0], {{0, submorphisms[1]}});
}

template <SymbolsRegistryLike SymbolsRegistry>
CookingResult<lang::MorphismPtr> Cooker<SymbolsRegistry>::CookSimpleMorphism(
    std::string raw_morphism) {
    raw_morphism = util::Strip(raw_morphism);

    if (raw_morphism.empty()) {
        return MakeCookingError("empty morphism is not allowed");
    }

    if (std::isdigit(raw_morphism.front())) {
        auto maybe_literal = CookLiteral(raw_morphism);

        if (!maybe_literal) {
            return std::unexpected(std::move(maybe_literal.error()));
        }

        return lang::Morphism::Literal(std::move(maybe_literal.value()));
    }

    if (raw_morphism == "+") {
        return lang::Morphism::Plus();
    } else if (raw_morphism == "-") {
        return lang::Morphism::Minus();
    } else if (raw_morphism == "*") {
        return lang::Morphism::Multiply();
    } else if (raw_morphism == ">") {
        return lang::Morphism::Greater();
    } else if (raw_morphism == "<") {
        return lang::Morphism::Less();
    } else if (raw_morphism == ">=") {
        return lang::Morphism::GreaterEq();
    } else if (raw_morphism == "<=") {
        return lang::Morphism::LessEq();
    } else if (raw_morphism == "id") {
        return lang::Morphism::Identity();
    }

    auto maybe_symbol = FindSymbol(raw_morphism);

    if (!maybe_symbol) {
        return MakeCookingError(std::format("symbol \"{}\" not found", raw_morphism));
    }

    SymbolInfo symbol_info = std::move(maybe_symbol.value());
    if (symbol_info.kind == SymbolKind::Function) {
        return lang::Morphism::CommonWithType(raw_morphism, symbol_info.type);
    } else if (symbol_info.kind == SymbolKind::Value) {
        return lang::Morphism::Common(raw_morphism, lang::Type::Singleton(), symbol_info.type);
    }

    return MakeCookingError("wrong symbol kind for morphism");
}

template <SymbolsRegistryLike SymbolsRegistry>
CookingResult<CookedBrancher> Cooker<SymbolsRegistry>::CookBrancher(
    const std::string& raw_brancher) {
    if (raw_brancher.empty()) {
        return MakeCookingError("empty brancher is not allowed");
    }

    if (raw_brancher[0] == '|') {
        return CookGuard(raw_brancher.substr(1, raw_brancher.size() - 1));
    }

    return CookPattern(raw_brancher);
}

template <SymbolsRegistryLike SymbolsRegistry>
CookingResult<lang::Pattern> Cooker<SymbolsRegistry>::CookPattern(std::string raw_pattern) {
    raw_pattern = util::Strip(raw_pattern);

    if (raw_pattern == "*") {
        return lang::Pattern::Any();
    }

    if (raw_pattern.front() == '(') {
        std::vector<lang::Pattern> subpatterns;
        int64_t balance = 0;
        int64_t last_start_idx = 1;

        auto mismatch_error =
            MakeCookingError(std::format("brackets mismatch in pattern \"{}\"", raw_pattern));

        for (size_t i = 0; i < raw_pattern.size(); ++i) {
            bool last = i + 1 == raw_pattern.size();

            if (raw_pattern[i] == '(') {
                ++balance;
            } else if (raw_pattern[i] == ')') {
                --balance;
            }

            if ((raw_pattern[i] == ',' && balance == 1) || last) {
                auto maybe_subpattern =
                    CookPattern(raw_pattern.substr(last_start_idx, i - last_start_idx));

                if (!maybe_subpattern) {
                    return std::unexpected(maybe_subpattern.error());
                }

                subpatterns.emplace_back(std::move(maybe_subpattern.value()));
                last_start_idx = i + 1;
            }

            if (balance == 0 && !last) {
                return mismatch_error;
            }
        }

        if (balance != 0) {
            return mismatch_error;
        }

        return lang::Pattern::Tuple(std::move(subpatterns));
    }

    auto maybe_value = CookLiteral(std::move(raw_pattern));

    if (!maybe_value) {
        return std::unexpected(maybe_value.error());
    }

    return lang::Pattern::FromLiteral(std::move(maybe_value.value()));
}

template <SymbolsRegistryLike SymbolsRegistry>
CookingResult<lang::Guard> Cooker<SymbolsRegistry>::CookGuard(const std::string& raw_guard) {
    auto maybe_morphism = CookMorphism(raw_guard);

    if (!maybe_morphism) {
        return std::unexpected(maybe_morphism.error());
    }

    lang::MorphismPtr morphism = std::move(maybe_morphism.value());

    if (morphism->GetTarget() != lang::Type::Bool()) {
        return MakeCookingError(std::format("expected guard to return \"Bool\" but got \"{}\"",
                                            morphism->GetTarget().ToString()));
    }

    return lang::Guard(std::move(morphism));
}

template <SymbolsRegistryLike SymbolsRegistry>
CookingResult<lang::Literal> Cooker<SymbolsRegistry>::CookLiteral(std::string raw_literal) {
    raw_literal = util::Strip(raw_literal);

    if (raw_literal[0] == '\'') {
        if (raw_literal.back() != '\'' || raw_literal.size() < 2) {
            return MakeCookingError(
                std::format("Missing ending quote in a character \"{}\"", raw_literal));
        }

        auto wrong_char_err = MakeCookingError(std::format("Wrong character \"{}\"", raw_literal));

        raw_literal = raw_literal.substr(1, raw_literal.size() - 2);
        if (raw_literal.empty() || raw_literal.size() > 3) {
            return wrong_char_err;
        }

        if (raw_literal.size() == 1) {
            return lang::Literal::Char(raw_literal[0]);
        }

        if (raw_literal == "\\n") {
            return lang::Literal::Char('\n');
        }

        return wrong_char_err;
    }

    auto maybe_num = CookNumber(raw_literal.c_str());

    if (!maybe_num) {
        return std::unexpected(maybe_num.error());
    }

    return lang::Literal::Number(maybe_num.value());
}

template <SymbolsRegistryLike SymbolsRegistry>
CookingResult<lang::CatProgram> Cook(const RawCatProgram& raw_program,
                                     const SymbolsRegistry& symbols_registry) {
    return Cooker(symbols_registry).Cook(raw_program);
}

// Cook function template instantiations

template CookingResult<lang::CatProgram> Cook<SimpleSymbolsRegistry>(
    const RawCatProgram& program, const SimpleSymbolsRegistry& param);

}  // namespace komaru::translate
