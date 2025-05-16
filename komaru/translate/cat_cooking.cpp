#include "cat_cooking.hpp"

#include <komaru/parsers/cat_type_parser.hpp>
#include <komaru/parsers/morphism_parser.hpp>
#include <komaru/parsers/pattern_parser.hpp>
#include <komaru/translate/simple_symbols_registry.hpp>
#include <komaru/util/string.hpp>
#include <komaru/util/std_extensions.hpp>

#include <format>
#include <cstdlib>
#include <print>
#include <optional>
#include <numeric>
#include <cassert>

namespace komaru::translate {

using CookedNode = lang::CatProgram::Node;
using CookedArrow = lang::CatProgram::Arrow;
using CookedOutPin = lang::CatProgram::OutPin;
using CookedBrancher = CookedOutPin::Brancher;

class Cooker {
public:
    explicit Cooker(const RawCatProgram& raw_program, hs::HaskellSymbolsRegistry& symbols_registry)
        : raw_program_(raw_program),
          symbols_registry_(symbols_registry) {
    }

    CookingResult<lang::CatProgram> Cook();

private:
    std::optional<CookingError> CookNodeTypes();
    CookingResult<std::vector<size_t>> DiscoverFunctions();
    std::optional<CookingError> AddFunctionsToRegistry(const std::vector<size_t>& root_ids);
    std::optional<CookingError> CookFunction(size_t root_id);
    std::optional<CookingError> CookNodeAndIncomingArrows(size_t node_id);

    CookingResult<lang::MorphismPtr> CookMorphism(const std::string& raw);
    CookingResult<CookedBrancher> CookBrancher(const std::string& raw);
    CookingResult<lang::Pattern> CookPattern(const std::string& raw);
    CookingResult<lang::Guard> CookGuard(const std::string& raw);

    void BuildGraphs();
    bool HasOrientedCycles() const;
    void AddSymbolsForPattern(const lang::Pattern& pattern, lang::Type type);

private:
    const RawCatProgram& raw_program_;
    hs::HaskellSymbolsRegistry& symbols_registry_;
    lang::CatProgramBuilder builder_;
    std::vector<lang::Type> node2type_;
    std::vector<CookedNode*> node2cooked_;
    std::vector<std::vector<std::pair<size_t, size_t>>> graph_;
    std::vector<std::vector<std::pair<size_t, size_t>>> rgraph_;
};

CookingResult<lang::CatProgram> Cooker::Cook() {
    BuildGraphs();

    if (HasOrientedCycles()) {
        return MakeCookingError("program can't have oriented cycles");
    }

    auto maybe_err = CookNodeTypes();
    if (maybe_err.has_value()) {
        return std::unexpected(maybe_err.value());
    }

    auto maybe_func_roots = DiscoverFunctions();
    if (!maybe_func_roots) {
        return std::unexpected(maybe_func_roots.error());
    }

    std::vector<size_t> func_roots = std::move(maybe_func_roots.value());

    maybe_err = AddFunctionsToRegistry(func_roots);
    if (maybe_err.has_value()) {
        return std::unexpected(maybe_err.value());
    }

    node2cooked_.resize(graph_.size(), nullptr);

    for (size_t root_id : func_roots) {
        auto maybe_err = CookFunction(root_id);
        if (maybe_err.has_value()) {
            return std::unexpected(maybe_err.value());
        }
    }

    return builder_.Extract();
}

std::optional<CookingError> Cooker::CookNodeTypes() {
    node2type_.reserve(raw_program_.GetNodes().size());

    for (const auto& node : raw_program_.GetNodes()) {
        auto maybe_type = parsers::CatTypeParser(node.type, symbols_registry_).Parse();
        if (!maybe_type) {
            return CookingError::From(maybe_type.error());
        }

        node2type_.push_back(std::move(maybe_type.value()));
    }

    return std::nullopt;
}

CookingResult<std::vector<size_t>> Cooker::DiscoverFunctions() {
    std::vector<size_t> root_ids;
    std::vector<size_t> node2root(rgraph_.size(), std::numeric_limits<size_t>::max());

    auto dfs = [&](auto& self, size_t v) -> bool {
        for (auto [u, _] : rgraph_[v]) {
            if (node2root[u] == std::numeric_limits<size_t>::max() && !self(self, u)) {
                return false;
            }

            if (node2root[v] != std::numeric_limits<size_t>::max() &&
                node2root[v] != node2root[u]) {
                return false;
            }

            node2root[v] = node2root[u];
        }

        if (node2root[v] == std::numeric_limits<size_t>::max()) {
            node2root[v] = v;
            root_ids.push_back(v);
        }

        return true;
    };

    for (size_t v = 0; v < node2root.size(); ++v) {
        if (node2root[v] == std::numeric_limits<size_t>::max()) {
            if (!dfs(dfs, v)) {
                return MakeCookingError("found multiple roots for a single function");
            }
        }
    }

    return root_ids;
}

std::optional<CookingError> Cooker::AddFunctionsToRegistry(const std::vector<size_t>& root_ids) {
    symbols_registry_.ResetGlobalFunctions();
    std::vector<lang::Type> node2ret(graph_.size());
    std::vector<char> vis(graph_.size(), false);

    auto dfs = [&](auto& self, size_t v) -> bool {
        vis[v] = true;

        if (graph_[v].empty()) {
            node2ret[v] = node2type_[v];
            return true;
        }

        std::optional<lang::Type> ret = std::nullopt;
        for (auto [u, _] : graph_[v]) {
            if (!vis[u]) {
                self(self, u);
            }
            if (!ret.has_value()) {
                ret = node2ret[u];
            } else if (ret.value() != node2ret[u]) {
                return false;
            }
        }
        node2ret[v] = ret.value();
        return true;
    };

    auto& nodes = raw_program_.GetNodes();

    for (size_t root_id : root_ids) {
        if (!dfs(dfs, root_id)) {
            return CookingError("found inconsistent return types for a single function");
        }

        if (!nodes[root_id].name.has_value()) {
            return CookingError("unnamed functions are not allowed");
        }
        auto func_type = lang::Type::Function(node2type_[root_id], node2ret[root_id]);

        symbols_registry_.AddGlobalFunction(nodes[root_id].name.value(), func_type.Pure());
    }

    return std::nullopt;
}

std::optional<CookingError> Cooker::CookFunction(size_t root_id) {
    symbols_registry_.ResetLocalFunctions();
    auto& nodes = raw_program_.GetNodes();
    std::vector<size_t> node2views(graph_.size(), 0);
    std::queue<size_t> normal_q;
    std::queue<size_t> branch_q;

    normal_q.push(root_id);

    while (!normal_q.empty() || !branch_q.empty()) {
        size_t node_id = 0;
        if (!normal_q.empty()) {
            node_id = normal_q.front();
            normal_q.pop();
        } else {
            node_id = branch_q.front();
            branch_q.pop();
        }

        auto maybe_err = CookNodeAndIncomingArrows(node_id);
        if (maybe_err) {
            return maybe_err;
        }

        for (auto [u, _] : graph_[node_id]) {
            ++node2views[u];
            if (node2views[u] < rgraph_[u].size()) {
                continue;
            }

            if (nodes[u].branchers.size() > 1) {
                branch_q.push(u);
            } else {
                normal_q.push(u);
            }
        }
    }

    return std::nullopt;
}

std::optional<CookingError> Cooker::CookNodeAndIncomingArrows(size_t node_id) {
    auto& nodes = raw_program_.GetNodes();
    auto& conns = raw_program_.GetConnections();
    auto& node = nodes[node_id];
    CookedNode& cnode = builder_.NewNode(node2type_[node_id], nodes[node_id].name.value_or(""));
    node2cooked_[node_id] = &cnode;

    if (!cnode.GetName().empty() && !rgraph_[node_id].empty()) {
        symbols_registry_.AddLocalFunction(cnode.GetName(), cnode.GetType());
    }

    for (auto [parent_id, conn_id] : rgraph_[node_id]) {
        CookedNode* parent_node = node2cooked_[parent_id];
        auto& conn = conns[conn_id];

        assert(conn.pin_idx < parent_node->OutPins().size());

        auto& out_pin = parent_node->OutPins()[conn.pin_idx];
        auto maybe_morphism = CookMorphism(conn.morphism);

        if (!maybe_morphism) {
            return maybe_morphism.error();
        }

        builder_.Connect(out_pin, cnode, std::move(maybe_morphism.value()));
    }

    for (const std::string& raw_brancher : node.branchers) {
        auto maybe_brancher = CookBrancher(raw_brancher);

        if (!maybe_brancher) {
            return maybe_brancher.error();
        }

        auto& brancher = maybe_brancher.value();

        if (std::holds_alternative<lang::Pattern>(brancher)) {
            AddSymbolsForPattern(std::get<lang::Pattern>(brancher), cnode.GetType());
        }

        std::visit(
            [&cnode](auto brancher) {
                cnode.AddOutPin(std::move(brancher));
            },
            std::move(brancher));
    }

    return std::nullopt;
}

CookingResult<lang::MorphismPtr> Cooker::CookMorphism(const std::string& raw) {
    auto maybe_morphism = parsers::MorphismParser(raw, symbols_registry_).Parse();
    if (!maybe_morphism) {
        return std::unexpected(CookingError::From(maybe_morphism.error()));
    }

    return std::move(maybe_morphism.value());
}

CookingResult<CookedBrancher> Cooker::CookBrancher(const std::string& raw) {
    size_t pos = 0;
    while (pos < raw.size() && std::isspace(raw[pos])) {
        ++pos;
    }

    if (pos == raw.size()) {
        return MakeCookingError("empty brancher");
    }

    if (raw[pos] == '|') {
        return CookGuard(raw.substr(pos + 1));
    }

    return CookPattern(raw);
}

CookingResult<lang::Pattern> Cooker::CookPattern(const std::string& raw) {
    auto maybe_pattern = parsers::PatternParser(raw, symbols_registry_).Parse();
    if (!maybe_pattern) {
        return std::unexpected(CookingError::From(maybe_pattern.error()));
    }

    return std::move(maybe_pattern.value());
}

CookingResult<lang::Guard> Cooker::CookGuard(const std::string& raw) {
    auto maybe_morphism = parsers::MorphismParser(raw, symbols_registry_).Parse();
    if (!maybe_morphism) {
        return MakeCookingError(maybe_morphism.error().Error());
    }

    auto& morphism = maybe_morphism.value();

    if (morphism->GetTarget() != lang::Type::Bool()) {
        return MakeCookingError("guard morphism must return bool");
    }

    return lang::Guard(std::move(morphism));
}

void Cooker::BuildGraphs() {
    graph_.resize(raw_program_.GetNodes().size());
    rgraph_.resize(raw_program_.GetNodes().size());

    for (auto [i, conn] : util::Enumerate(raw_program_.GetConnections())) {
        graph_[conn.source_node].emplace_back(conn.target_node, i);
        rgraph_[conn.target_node].emplace_back(conn.source_node, i);
    }
}

bool Cooker::HasOrientedCycles() const {
    std::vector<char> color(graph_.size(), 0);

    auto dfs = [&](auto& self, size_t v) -> bool {
        color[v] = 1;
        for (auto [u, _] : graph_[v]) {
            if (color[u] == 1) {
                return true;
            }

            if (!color[u] && self(self, u)) {
                return true;
            }
        }
        color[v] = 2;
        return false;
    };

    for (size_t v = 0; v < graph_.size(); ++v) {
        if (!color[v] && dfs(dfs, v)) {
            return true;
        }
    }

    return false;
}

void Cooker::AddSymbolsForPattern(const lang::Pattern& pattern, lang::Type node_type) {
    auto mapping = pattern.GetNamesMapping(symbols_registry_, node_type);
    for (auto&& [name, type] : mapping) {
        symbols_registry_.AddLocalFunction(name, type);
    }
}

CookingResult<lang::CatProgram> Cook(const RawCatProgram& raw_program,
                                     hs::HaskellSymbolsRegistry& symbols_registry) {
    return Cooker(raw_program, symbols_registry).Cook();
}

}  // namespace komaru::translate
