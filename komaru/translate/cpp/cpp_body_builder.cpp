#include "cpp_body_builder.hpp"

#include <util/defer.hpp>

#include <format>

namespace komaru::translate::cpp {

const std::vector<CppBranches::Branch>& CppBranches::GetBranches() const {
    return branches_;
}

CppScope::CppScope(CppCond cond)
    : cond_(std::move(cond)) {}

void CppScope::GrowBody(const std::string& s) {
    body_ += s;
}

void CppScope::SetBranches(CppBranches branches) {
    branches_ = std::move(branches);
}

const CppCond& CppScope::GetCond() const {
    return cond_;
}

const std::string& CppScope::GetBody() const {
    return body_;
}

const std::optional<CppBranches>& CppScope::GetBranches() const {
    return branches_;
}

CppBodyBuilder::CppBodyBuilder() {
    scopes_.emplace_back(CppCond{});
    active_scopes_.push_front(&scopes_[0]);
}

void CppBodyBuilder::AddStatement(const CppCond& cond, std::string statement) {
    statement += ";\n";

    for(auto* scope : active_scopes_) {
        // TODO: If node's input arrow is reachable by the output pin of the corresponding if branch <=> implication works?
        // Test with a bunch of examples for proof by AC :)
        if(scope->GetCond().DoesImply(cond)) {
            scope->GrowBody(statement);
        }
    }
}

void CppBodyBuilder::AddReturn(const CppCond& cond, std::string statement) {
    statement += ";\n";

    for(auto it = active_scopes_.begin(); it != active_scopes_.end();) {
        auto& scope = **it;

        if(scope.GetCond().DoesImply(cond)) {
            scope.GrowBody(statement);
            it = active_scopes_.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<CppCond> CppBodyBuilder::AddBranches(const CppCond& cond, const std::vector<std::string>& branch_exprs) {
    std::vector<CppCond> conds(branch_exprs.size());

    int32_t start_branch_id = branch_id_;

    for(size_t i = 0; i < branch_exprs.size(); ++i) {
        conds[i] = cond & CppCond(branch_id_++);
    }

    for(auto it = active_scopes_.begin(); it != active_scopes_.end();) {
        auto& scope = **it;

        if(scope.GetCond().DoesImply(cond)) {
            std::vector<CppScope*> new_scopes;

            for(size_t i = 0; i < branch_exprs.size(); ++i) {
                int32_t branch_id = start_branch_id + static_cast<int32_t>(i);
                scopes_.emplace_back(scope.GetCond() & CppCond(branch_id));
                new_scopes.push_back(&scopes_.back());
            }

            scope.SetBranches(CppBranches(branch_exprs, new_scopes));

            it = active_scopes_.erase(it);
            active_scopes_.insert_range(it, new_scopes);
        } else {
            ++it;
        }
    }

    return conds;
}

TranslationResult<std::string> CppBodyBuilder::Extract() {
    Defer _([this](){
        Reset();
    });

    if(!active_scopes_.empty()) {
        return MakeTranslationError("there must be no active scopes when extracting a valid function body");
    }

    std::string res = ExtractImpl(&scopes_[0]);
    return res;
}

void CppBodyBuilder::Reset() {
    this->~CppBodyBuilder();
    new (this) CppBodyBuilder();
}

std::string CppBodyBuilder::ExtractImpl(const CppScope* scope) {
    std::string body = scope->GetBody();

    auto maybe_branches = scope->GetBranches();

    if(!maybe_branches.has_value()) {
        return body;
    }

    for(const auto& [branch_expr, branch_scope] : maybe_branches->GetBranches()) {
        std::string branch_body = ExtractImpl(branch_scope);

        body += std::format("if ({}) {{\n{}}}\n", branch_expr, branch_body);
    }

    return body;
}

}
