#pragma once
#include <komaru/translate/translator.hpp>
#include <komaru/translate/cpp/cpp_program_builder.hpp>
#include <komaru/translate/cpp/cpp_body_builder.hpp>
#include <komaru/translate/common/cond.hpp>
#include <komaru/translate/cpp/cpp_expr.hpp>

#include <unordered_map>
#include <filesystem>

namespace komaru::translate::cpp {

class CppTranslator : public ITranslator {
    using CPNode = lang::CatProgram::Node;
    using CPOutPin = lang::CatProgram::OutPin;
    using CPArrow = lang::CatProgram::Arrow;

public:
    explicit CppTranslator(const std::filesystem::path& catlib_dir);

    TranslationResult<std::unique_ptr<IProgram>> Translate(
        const lang::CatProgram& cat_prog) override;

private:
    void LoadCatlib();

    TranslationResult<CppFunction> TranslateMorphismGraph(const CPNode* root);

    bool IsIntersectionNode(const CPNode* node);
    void AddStatementsForNode(CppBodyBuilder& body_builder, const common::Cond& node_cond,
                              const CPNode* node, const std::string& local_name);
    CppExpr MakeExprForIntersectionNode(const CPNode* node);
    CppExpr MakeExprForArrow(const CPArrow* arrow);

    CppExpr MakeExprForBrancher(const CPOutPin::Brancher& brancher, const CppExpr& in_expr);
    CppExpr MakeExprForGuard(const lang::Guard& guard, const CppExpr& in_expr);
    CppExpr MakeExprForPattern(const lang::Pattern& pattern, const CppExpr& in_expr);
    CppExpr MakeExprForPattern(const lang::AnyPattern& pattern, const CppExpr& in_expr);
    CppExpr MakeExprForPattern(const lang::LiteralPattern& pattern, const CppExpr& in_expr);
    CppExpr MakeExprForPattern(const lang::NamePattern& pattern, const CppExpr& in_expr);
    CppExpr MakeExprForPattern(const lang::ConstructorPattern& pattern, const CppExpr& in_expr);
    CppExpr MakeExprForPattern(const lang::TuplePattern& pattern, const CppExpr& in_expr);

    CppExpr MakeExprForMorphism(const lang::Morphism& morphism, const CppExpr& in_expr,
                                lang::Type out_type);
    CppExpr MakeExprForMorphism(const lang::CommonMorphism& morphism, const CppExpr& in_expr,
                                lang::Type out_type);
    CppExpr MakeExprForMorphism(const lang::BindedMorphism& morphism, const CppExpr& in_expr,
                                lang::Type out_type);
    CppExpr MakeExprForMorphism(const lang::LiteralMorphism& morphism, const CppExpr& in_expr,
                                lang::Type out_type);
    CppExpr MakeExprForMorphism(const lang::TupleMorphism& morphism, const CppExpr& in_expr,
                                lang::Type out_type);

    std::vector<std::string> MakeBranchExprs(const CPNode* node);
    std::string MakeStatement(lang::Type type, const std::string& var_name,
                              const std::string& expr);
    common::Cond CalcCppCondForNode(const CPNode* node);

    std::optional<TranslationError> CalcRoots(const lang::CatProgram& cat_program);
    std::optional<const CPNode*> GetRootOrCalcIt(const CPNode* node);
    const CPNode* GetRoot(const CPNode* node);

    TranslationResult<std::vector<const CPNode*>> DiscoverFunctions(
        const lang::CatProgram& cat_prog);

    std::string ToCppName(const std::string& name);

    void Reset();

private:
    CppProgramBuilder builder_;
    std::filesystem::path catlib_dir_;
    std::unordered_map<const CPNode*, const CPNode*> node2root_;
    std::unordered_map<const CPNode*, size_t> node2views_;
    std::unordered_map<const CPNode*, std::string> node2local_name_;
    std::unordered_map<const CPOutPin*, common::Cond> pin2cond_;
    std::unordered_map<std::string, lang::Type> local_name2type_;
    std::unordered_map<std::string, lang::Type> global_name2type_;

    static const std::unordered_map<std::string, std::string> kNameConv;
    static const std::unordered_set<std::string> kDeductionSet;
};

}  // namespace komaru::translate::cpp
