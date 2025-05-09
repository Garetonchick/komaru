#pragma once

#include <komaru/translate/translator.hpp>
#include <komaru/translate/haskell/hs_definition.hpp>
#include <komaru/translate/common/cond.hpp>
#include <komaru/translate/haskell/hs_expr_builder.hpp>

namespace komaru::translate::hs {

class HaskellFuncTranslationRequest {
    using CPNode = lang::CatProgram::Node;
    using CPOutPin = lang::CatProgram::OutPin;
    using CPArrow = lang::CatProgram::Arrow;
    using CPBrancher = CPOutPin::Brancher;

public:
    explicit HaskellFuncTranslationRequest(const lang::CatProgram::Node& root)
        : root_(root) {
    }

    TranslationResult<HaskellDefinition> Translate() &&;

private:
    std::optional<TranslationError> Init();
    std::optional<TranslationError> DeduceNodeAndArrowTypes();
    std::optional<TranslationError> DeduceConnType(const CPNode* src_node, const CPArrow* arrow,
                                                   const CPNode* dst_node);
    void CalcParamNames();
    TranslationResult<HaskellExpr> TranslateExpr();

    common::Cond CalcCondForNode(const CPNode* node);
    std::optional<TranslationError> AddDefinitionsForNode(const CPNode* node,
                                                          const common::Cond& node_cond,
                                                          const std::string& local_name);
    std::optional<TranslationError> AddDefinitionsForIntersectionNode(
        const CPNode* node, const common::Cond& node_cond, const std::string& local_name);
    std::optional<TranslationError> AddDefinitionsForNormalNode(const CPNode* node,
                                                                const common::Cond& node_cond,
                                                                const std::string& local_name);
    std::optional<TranslationError> AddBranchesForNode(const CPNode* node,
                                                       const common::Cond& node_cond,
                                                       const std::string& local_name);
    std::string NameNode(const CPNode* node);
    std::string NewVarName();
    bool HasNonTrivialBrancher(const CPNode* node) const;
    bool IsBrancherNode(const CPNode* node) const;
    bool IsIntersectionNode(const CPNode* node) const;
    bool IsNormalNode(const CPNode* node) const;
    TranslationResult<std::vector<HaskellBrancher>> MakeHaskellBranchers(
        const CPNode* node, const std::string& local_name);
    TranslationResult<HaskellDefinition> MakeHaskellDefinitionForArrow(
        const CPArrow* arrow, const std::string& local_name);

private:
    const lang::CatProgram::Node& root_;
    lang::Type func_type_;
    lang::Type ret_type_;
    HaskellExprBuilder expr_builder_;
    std::unordered_map<const CPNode*, lang::Type> node2deduced_type_;
    std::unordered_map<const CPNode*, std::string> node2local_name_;
    std::unordered_map<const CPNode*, std::vector<std::string>> node2unpack_;
    std::unordered_map<const CPArrow*, lang::Type> arrow2deduced_type_;
    std::unordered_map<const CPOutPin*, common::Cond> pin2cond_;
    std::vector<std::string> param_names_;
    size_t var_idx_{0};
};

}  // namespace komaru::translate::hs
