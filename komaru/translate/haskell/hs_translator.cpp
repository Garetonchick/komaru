#include "hs_translator.hpp"

#include <komaru/translate/haskell/hs_program_builder.hpp>
#include <komaru/translate/haskell/hs_func_translation_req.hpp>

#include <unordered_set>

namespace komaru::translate::hs {

using CPNode = lang::CatProgram::Node;
using CPOutPin = lang::CatProgram::OutPin;
using CPArrow = lang::CatProgram::Arrow;

class HaskellTranslationRequest {
public:
    explicit HaskellTranslationRequest(const lang::CatProgram& cat_prog)
        : cat_prog_(cat_prog) {
    }

    ResultProgram Translate() &&;

private:
    std::vector<const CPNode*> DiscoverFunctions();
    lang::Type FindReturnType(const CPNode* node);

private:
    const lang::CatProgram& cat_prog_;
};

ResultProgram HaskellTranslationRequest::Translate() && {
    HaskellProgramBuilder builder;

    builder.AddPragma("MultiWayIf");
    builder.AddImport("Control.Monad");

    lang::Type main_type = lang::Type::Parameterized("IO", {lang::Type::Singleton()});
    bool is_interpreter_mode = false;

    auto roots = DiscoverFunctions();
    for (const auto& root : roots) {
        if (root->GetName() == "main" && FindReturnType(root) != main_type) {
            is_interpreter_mode = true;
        }

        auto maybe_def = HaskellFuncTranslationRequest(*root).Translate();
        if (!maybe_def) {
            return std::unexpected(maybe_def.error());
        }

        builder.AddDefinition(std::move(maybe_def.value()));
    }

    if (is_interpreter_mode) {
        builder.ChangeDefinitionName("main", "catMain");

        auto main_def = HaskellDefinition::Normal("main", {}, main_type,
                                                  HaskellExpr::Simple(main_type, "print catMain"));

        builder.AddDefinition(std::move(main_def));
    }

    return builder.Extract();
}

std::vector<const CPNode*> HaskellTranslationRequest::DiscoverFunctions() {
    std::unordered_map<const CPNode*, const CPNode*> node2root;

    auto find_root = [&](auto&& self, const CPNode* node) -> const CPNode* {
        if (node->IncomingArrows().empty()) {
            return node;
        }

        auto it = node2root.find(node);
        if (it != node2root.end()) {
            return it->second;
        }

        auto root = self(self, &node->IncomingArrows().front()->SourcePin().GetNode());
        node2root[node] = root;
        return root;
    };

    std::unordered_set<const CPNode*> roots;
    for (const auto& node : cat_prog_.GetNodes()) {
        roots.insert(find_root(find_root, &node));
    }

    return std::vector<const CPNode*>(roots.begin(), roots.end());
}

lang::Type HaskellTranslationRequest::FindReturnType(const CPNode* node) {
    while (!node->OutPins().empty() && !node->OutPins().front().Arrows().empty()) {
        node = &node->OutPins().front().Arrows().front().TargetNode();
    }

    return node->GetType();
}

HaskellTranslator::HaskellTranslator() {
}

ResultProgram HaskellTranslator::Translate(const lang::CatProgram& cat_prog) {
    return HaskellTranslationRequest(cat_prog).Translate();
}

}  // namespace komaru::translate::hs
