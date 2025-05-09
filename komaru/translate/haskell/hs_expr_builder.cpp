#include "hs_expr_builder.hpp"

#include <komaru/util/std_extensions.hpp>
#include <komaru/util/string.hpp>
#include <sstream>

namespace komaru::translate::hs {

Scope::Scope(common::Cond cond)
    : cond_(std::move(cond)) {
}

void Scope::AddDefinition(HaskellDefinition definition) {
    definitions_.push_back(std::move(definition));
}

void Scope::SetBranches(Branches branches) {
    branches_ = std::move(branches);
}

const common::Cond& Scope::GetCond() const {
    return cond_;
}

const std::vector<HaskellDefinition>& Scope::GetDefinitions() const {
    return definitions_;
}

const std::optional<Scope::Branches>& Scope::GetBranches() const {
    return branches_;
}

HaskellExprBuilder::HaskellExprBuilder() {
    scopes_.emplace_back(common::Cond{});
    active_scopes_.push_front(&scopes_[0]);
}

void HaskellExprBuilder::AddDefinition(const common::Cond& cond, HaskellDefinition definition) {
    for (Scope* scope : active_scopes_) {
        if (scope->GetCond().DoesImply(cond)) {
            scope->AddDefinition(definition);
        }
    }
}

void HaskellExprBuilder::CloseScopes(const common::Cond& cond) {
    for (auto it = active_scopes_.begin(); it != active_scopes_.end();) {
        Scope* scope = *it;
        if (scope->GetCond().DoesImply(cond)) {
            it = active_scopes_.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<common::Cond> HaskellExprBuilder::AddBranches(
    const common::Cond& cond, const std::vector<HaskellBrancher>& branchers) {
    std::vector<common::Cond> conds(branchers.size());

    int32_t start_branch_id = branch_id_;

    for (size_t i = 0; i < branchers.size(); ++i) {
        conds[i] = cond & common::Cond(branch_id_++);
    }

    for (auto it = active_scopes_.begin(); it != active_scopes_.end();) {
        Scope& scope = **it;

        if (!scope.GetCond().DoesImply(cond)) {
            ++it;
            continue;
        }

        Scope::Branches branches;

        for (size_t i = 0; i < branchers.size(); ++i) {
            int32_t branch_id = start_branch_id + static_cast<int32_t>(i);
            scopes_.emplace_back(scope.GetCond() & common::Cond(branch_id));
            branches.emplace_back(&scopes_.back(), branchers[i]);
        }

        scope.SetBranches(branches);
    }

    return conds;
}

TranslationResult<HaskellExpr> HaskellExprBuilder::Extract() {
    if (!active_scopes_.empty()) {
        return MakeTranslationError(
            "there must be no active scopes when extracting a valid expression");
    }

    return ExtractImpl(scopes_[0]);
}

TranslationResult<HaskellExpr> HaskellExprBuilder::ExtractImpl(const Scope& scope) {
    std::stringstream ss;

    const auto& definitions = scope.GetDefinitions();
    const auto& maybe_branches = scope.GetBranches();

    if (definitions.empty()) {
        return MakeTranslationError("scope must have at least one definition");
    }

    std::string indent = util::k2S;

    ss << "let\n";
    for (const auto& definition : definitions) {
        ss << util::Indent(definition.ToString(), indent) << "\n";
    }
    ss << "in\n";

    if (!maybe_branches) {
        ss << util::Indent(definitions.back().GetName(), indent);
        return HaskellExpr::Simple(definitions.back().GetType(), ss.str());
    }

    const auto& branches = maybe_branches.value();

    ss << indent << "if\n";

    indent += util::k2S;

    lang::Type type = lang::Type::Auto();

    for (const auto& branch : branches) {
        auto maybe_subexpr = ExtractImpl(*branch.scope);
        if (!maybe_subexpr) {
            return maybe_subexpr;
        }

        if (type == lang::Type::Auto()) {
            type = maybe_subexpr.value().GetType();
        }

        ss << indent << "| " << branch.brancher.ToString() << " ->\n";
        ss << util::Indent(maybe_subexpr.value().ToString(), indent + util::k2S);
    }

    return HaskellExpr::Simple(type, ss.str());
}

}  // namespace komaru::translate::hs
