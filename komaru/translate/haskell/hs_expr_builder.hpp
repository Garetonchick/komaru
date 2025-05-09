#pragma once

#include <komaru/translate/haskell/hs_expr.hpp>
#include <komaru/translate/haskell/hs_definition.hpp>
#include <komaru/translate/haskell/hs_brancher.hpp>
#include <komaru/translate/translator.hpp>
#include <komaru/translate/common/cond.hpp>

#include <deque>
#include <list>

namespace komaru::translate::hs {

class Scope {
public:
    struct Branch {
        Scope* scope;
        HaskellBrancher brancher;
    };

    using Branches = std::vector<Branch>;

public:
    explicit Scope(common::Cond cond);

    void AddDefinition(HaskellDefinition definition);
    void SetBranches(Branches branches);

    const common::Cond& GetCond() const;
    const std::vector<HaskellDefinition>& GetDefinitions() const;
    const std::optional<Branches>& GetBranches() const;

private:
    common::Cond cond_;
    std::vector<HaskellDefinition> definitions_;
    std::optional<Branches> branches_;
};

class HaskellExprBuilder {
public:
    HaskellExprBuilder();

    void AddDefinition(const common::Cond& cond, HaskellDefinition definition);
    void CloseScopes(const common::Cond& cond);
    std::vector<common::Cond> AddBranches(const common::Cond& cond,
                                          const std::vector<HaskellBrancher>& branchers);

    TranslationResult<HaskellExpr> Extract();

private:
    TranslationResult<HaskellExpr> ExtractImpl(const Scope& scope);

private:
    std::deque<Scope> scopes_;
    std::list<Scope*> active_scopes_;
    int32_t branch_id_{0};
};

}  // namespace komaru::translate::hs
