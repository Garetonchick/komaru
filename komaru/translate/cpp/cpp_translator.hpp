#pragma once
#include <komaru/translate/translator.hpp>
#include <komaru/translate/cpp/cpp_program_builder.hpp>
#include <komaru/translate/cpp/cpp_body_builder.hpp>
#include <komaru/translate/cpp/cpp_cond.hpp>

#include <unordered_map>

namespace komaru::translate::cpp {

class CppTranslator : public ITranslator {
    using CPNode = lang::CatProgram::Node;
    using CPOutPin = lang::CatProgram::OutPin;
    using CPArrow = lang::CatProgram::Arrow;

public:
    TranslationResult<std::unique_ptr<IProgram>> Translate(
        const lang::CatProgram& cat_prog) override;

private:
    TranslationResult<CppFunction> TranslateMorphismGraph(const CPNode* root);

    bool IsIntersectionNode(const CPNode* node);
    void AddStatementsForNode(CppBodyBuilder& body_builder, const CppCond& node_cond,
                              const CPNode* node, const std::string& local_name);
    std::string MakeExprForIntersectionNode(const CPNode* node);
    std::string MakeExprForArrow(const CPArrow* arrow);

    std::string MakeExprForBrancher(const CPOutPin::Brancher& brancher,
                                    const std::string& arg_name);
    std::string MakeExprForGuard(const lang::Guard& guard, const std::string& arg_name);
    std::string MakeExprForPattern(const lang::Pattern& pattern, const std::string& arg_name);
    std::string MakeExprForPattern(const lang::AnyPattern& pattern, const std::string& arg_name);
    std::string MakeExprForPattern(const lang::ValuePattern& pattern, const std::string& arg_name);
    std::string MakeExprForPattern(const lang::TuplePattern& pattern, const std::string& arg_name);

    std::string MakeExprForMorphism(const lang::Morphism& morphism, const std::string& arg_name);
    std::string MakeExprForMorphism(const lang::CompoundMorphism& morphism,
                                    const std::string& arg_name);
    std::string MakeExprForMorphism(const lang::BuiltinMorphism& morphism,
                                    const std::string& arg_name);
    std::string MakeExprForMorphism(const lang::ValueMorphism& morphism,
                                    const std::string& arg_name);
    std::string MakeExprForMorphism(const lang::BindedMorphism& morphism,
                                    const std::string& arg_name);
    std::string MakeExprForMorphism(const lang::NameMorphism& morphism,
                                    const std::string& arg_name);

    std::vector<std::string> MakeBranchExprs(const CPNode* node);
    std::string MakeStatement(lang::Type type, const std::string& var_name,
                              const std::string& expr);
    CppCond CalcCppCondForNode(const CPNode* node);

    std::optional<TranslationError> CalcRoots(const lang::CatProgram& cat_program);
    std::optional<const CPNode*> GetRootOrCalcIt(const CPNode* node);
    const CPNode* GetRoot(const CPNode* node);

    TranslationResult<std::vector<const CPNode*>> DiscoverFunctions(
        const lang::CatProgram& cat_prog);

    void Reset();

private:
    CppProgramBuilder builder_;
    std::unordered_map<const CPNode*, const CPNode*> node2root_;
    std::unordered_map<const CPNode*, size_t> node2views_;
    std::unordered_map<const CPNode*, std::string> node2local_name_;
    std::unordered_map<const CPOutPin*, CppCond> pin2cond_;
    std::unordered_map<std::string, lang::Type> local_name2type_;
    std::unordered_map<std::string, lang::Type> global_name2type_;

    static const std::unordered_map<std::string, std::string> kNameConv;
};

}  // namespace komaru::translate::cpp
