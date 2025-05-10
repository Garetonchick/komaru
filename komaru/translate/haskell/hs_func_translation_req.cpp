#include "hs_func_translation_req.hpp"

#include <komaru/translate/haskell/hs_definition.hpp>
#include <komaru/util/std_extensions.hpp>
#include <queue>
#include <format>
#include <cassert>

namespace komaru::translate::hs {

TranslationResult<HaskellDefinition> HaskellFuncTranslationRequest::Translate() && {
    auto maybe_err = Init();
    if (maybe_err.has_value()) {
        return std::unexpected(std::move(maybe_err.value()));
    }

    auto maybe_expr = TranslateExpr();
    if (!maybe_expr) {
        return std::unexpected(std::move(maybe_expr.error()));
    }

    auto expr = std::move(maybe_expr.value());

    return HaskellDefinition::Normal(std::move(root_.GetName()), std::move(param_names_),
                                     std::move(func_type_), std::move(expr));
}

std::optional<TranslationError> HaskellFuncTranslationRequest::Init() {
    auto maybe_err = DeduceNodeAndArrowTypes();

    if (maybe_err.has_value()) {
        return maybe_err.value();
    }

    CalcParamNames();

    return std::nullopt;
}

std::optional<TranslationError> HaskellFuncTranslationRequest::DeduceNodeAndArrowTypes() {
    std::unordered_map<const CPNode*, size_t> node2views;

    std::queue<const CPNode*> q;
    q.push(&root_);
    node2deduced_type_[&root_] = root_.GetType();

    while (!q.empty()) {
        const CPNode* node = q.front();
        q.pop();

        for (const CPArrow* arrow : node->IncomingArrows()) {
            const CPNode* src_node = &arrow->SourcePin().GetNode();
            auto maybe_err = DeduceConnType(src_node, arrow, node);
            if (maybe_err.has_value()) {
                return maybe_err.value();
            }
        }

        for (const CPOutPin& out_pin : node->OutPins()) {
            for (const CPArrow& arrow : out_pin.Arrows()) {
                const CPNode& dst_node = arrow.TargetNode();
                size_t n_views = (node2views[&dst_node] += 1);

                if (n_views == dst_node.IncomingArrows().size()) {
                    q.push(&dst_node);
                }
            }
        }

        if (node->OutPins().empty()) {
            ret_type_ = node2deduced_type_[node];
        }
    }

    func_type_ = lang::CurryFunction(root_.GetType(), ret_type_);
    return std::nullopt;
}

std::optional<TranslationError> HaskellFuncTranslationRequest::DeduceConnType(
    const CPNode* src_node, const CPArrow* arrow, const CPNode* dst_node) {
    lang::Type src_type = node2deduced_type_[src_node];
    lang::Type arrow_type =
        util::GetOr(arrow2deduced_type_, arrow, arrow->GetMorphism()->GetType());
    lang::Type dst_type = util::GetOr(node2deduced_type_, dst_node, dst_node->GetType());

    if (arrow->GetMorphism()->Holds<lang::PositionMorphism>()) {
        // TODO: Deduce dst_type for position morphism
        node2deduced_type_[dst_node] = dst_type;
        arrow2deduced_type_[arrow] = arrow_type;
        return std::nullopt;
    }

    std::map<size_t, lang::Type> arg_mapping;
    std::vector<lang::Type> src_components = src_type.GetComponents();
    for (size_t i = 0; i < src_components.size(); ++i) {
        arg_mapping[i] = src_components[i];
    }

    auto maybe_deduced_dst_type = lang::TryMakeSubstitution(arrow_type, arg_mapping);
    if (!maybe_deduced_dst_type.has_value()) {
        return TranslationError(
            std::format("failed to deduce destination type for arrow {}"
                        "\narrow type: {}, type mapping: {}",
                        lang::CurryFunction(src_type, dst_type).ToString(), arrow_type.ToString(),
                        lang::ArgMappingToString(arg_mapping)));
    }

    lang::Type deduced_dst_type = maybe_deduced_dst_type.value();

    auto maybe_match_map = lang::TryMatchTypes(dst_type, deduced_dst_type);
    if (!maybe_match_map.has_value()) {
        return TranslationError(
            std::format("failed to match types for arrow {} -> {}"
                        "\ndestination type: {}, deduced destination type: {}",
                        src_type.ToString(), dst_type.ToString(), dst_type.ToString(),
                        deduced_dst_type.ToString()));
    }

    deduced_dst_type = lang::ApplyMatchMap(dst_type, maybe_match_map.value());
    node2deduced_type_[dst_node] = deduced_dst_type;

    lang::Type conn_type = lang::CurryFunction(src_type, deduced_dst_type);

    maybe_match_map = lang::TryMatchTypes(arrow_type, conn_type);
    if (!maybe_match_map.has_value()) {
        return TranslationError(std::format("failed to match types for arrow {} -> {}",
                                            src_type.ToString(), dst_type.ToString()));
    }

    arrow_type = lang::ApplyMatchMap(arrow_type, maybe_match_map.value());
    arrow2deduced_type_[arrow] = arrow_type;

    return std::nullopt;
}

void HaskellFuncTranslationRequest::CalcParamNames() {
    if (root_.GetType() == lang::Type::Singleton()) {
        return;
    }

    size_t num_params = root_.GetType().GetComponentsNum();

    for (size_t i = 0; i < num_params; ++i) {
        param_names_.push_back("catPm" + std::to_string(i));
    }

    node2unpack_[&root_] = param_names_;
}

TranslationResult<HaskellExpr> HaskellFuncTranslationRequest::TranslateExpr() {
    std::unordered_map<const CPNode*, size_t> node2views;
    std::queue<const CPNode*> normal_q;
    std::queue<const CPNode*> branch_q;

    normal_q.push(&root_);

    if (root_.GetType() != lang::Type::Singleton()) {
        expr_builder_.AddDefinition(
            {}, HaskellDefinition::Normal("catArgs", {}, root_.GetType(),
                                          HaskellExpr::NamesTuple(root_.GetType(), param_names_)));
    }

    while (!normal_q.empty() || !branch_q.empty()) {
        const CPNode* node = nullptr;
        if (!normal_q.empty()) {
            node = normal_q.front();
            normal_q.pop();
        } else {
            node = branch_q.front();
            branch_q.pop();
        }

        common::Cond node_cond = CalcCondForNode(node);

        if (node->OutPins().size() == 1) {
            pin2cond_[&node->OutPins().front()] = node_cond;
        }

        std::string local_name = NameNode(node);

        auto maybe_err = AddDefinitionsForNode(node, node_cond, local_name);
        if (maybe_err.has_value()) {
            return std::unexpected(std::move(maybe_err.value()));
        }

        if (HasNonTrivialBrancher(node)) {
            auto maybe_err = AddBranchesForNode(node, node_cond, local_name);
            if (maybe_err.has_value()) {
                return std::unexpected(std::move(maybe_err.value()));
            }
        }

        if (node->OutPins().empty()) {
            expr_builder_.CloseScopes(node_cond);
        }

        for (const CPOutPin& out_pin : node->OutPins()) {
            for (const CPArrow& arrow : out_pin.Arrows()) {
                const CPNode& dst_node = arrow.TargetNode();
                size_t n_views = (node2views[&dst_node] += 1);
                if (n_views != dst_node.IncomingArrows().size()) {
                    continue;
                }

                if (IsBrancherNode(&dst_node)) {
                    branch_q.push(&dst_node);
                } else {
                    normal_q.push(&dst_node);
                }
            }
        }
    }

    return expr_builder_.Extract();
}

common::Cond HaskellFuncTranslationRequest::CalcCondForNode(const CPNode* node) {
    common::Cond node_cond;

    for (auto [i, arrow] : util::Enumerate(node->IncomingArrows())) {
        const CPOutPin* pin = &arrow->SourcePin();

        if (i == 0) {
            node_cond = pin2cond_[pin];
            continue;
        }

        if (arrow->GetMorphism()->Holds<lang::PositionMorphism>()) {
            node_cond &= pin2cond_[pin];
        } else {
            node_cond |= pin2cond_[pin];
        }
    }

    return node_cond;
}

std::optional<TranslationError> HaskellFuncTranslationRequest::AddDefinitionsForNode(
    const CPNode* node, const common::Cond& node_cond, const std::string& local_name) {
    if (IsIntersectionNode(node)) {
        return AddDefinitionsForIntersectionNode(node, node_cond, local_name);
    }
    if (IsNormalNode(node)) {
        return AddDefinitionsForNormalNode(node, node_cond, local_name);
    }

    return TranslationError(
        "positional morphisms can't be mixed with other morphisms for the same target node");
}

std::optional<TranslationError> HaskellFuncTranslationRequest::AddDefinitionsForIntersectionNode(
    const CPNode* node, const common::Cond& node_cond, const std::string& local_name) {
    std::vector<std::string> names(node->IncomingArrows().size());

    for (const CPArrow* arrow : node->IncomingArrows()) {
        assert(arrow->GetMorphism()->Holds<lang::PositionMorphism>());
        const auto& pos_morphism = arrow->GetMorphism()->GetVariant<lang::PositionMorphism>();

        if (pos_morphism.IsNonePosition()) {
            continue;
        }

        size_t pos = pos_morphism.GetPosition();
        if (pos >= names.size() || !names[pos].empty()) {
            return TranslationError("position morphisms don't form full tuple");
        }

        names[pos] = node2local_name_[&arrow->SourcePin().GetNode()];
    }

    while (!names.empty() && names.back().empty()) {
        names.pop_back();
    }

    if (names.empty()) {
        return std::nullopt;
    }

    for (const auto& name : names) {
        if (name.empty()) {
            return TranslationError("positional morphisms don't form full tuple");
        }
    }

    lang::Type node_type = node2deduced_type_[node];

    if (node_type.GetComponentsNum() != names.size()) {
        return TranslationError("positional morphisms don't form full tuple");
    }

    auto expr = HaskellExpr::NamesTuple(node_type, names);
    expr_builder_.AddDefinition(
        node_cond, HaskellDefinition::Normal(local_name, {}, node_type, std::move(expr)));

    node2unpack_[node] = names;

    return std::nullopt;
}

std::optional<TranslationError> HaskellFuncTranslationRequest::AddDefinitionsForNormalNode(
    const CPNode* node, const common::Cond& node_cond, const std::string& local_name) {
    lang::Type node_type = node2deduced_type_[node];

    for (const CPArrow* arrow : node->IncomingArrows()) {
        const CPOutPin* pin = &arrow->SourcePin();
        auto maybe_def = MakeHaskellDefinitionForArrow(arrow, local_name);
        if (!maybe_def.has_value()) {
            return maybe_def.error();
        }

        expr_builder_.AddDefinition(pin2cond_[pin], std::move(maybe_def.value()));
    }

    if (node_type.GetComponentsNum() > 1 && !node->OutPins().empty()) {
        std::vector<std::string> names(node_type.GetComponentsNum());
        for (size_t i = 0; i < names.size(); ++i) {
            names[i] = NewVarName();
        }

        node2unpack_[node] = names;
        expr_builder_.AddDefinition(
            node_cond, HaskellDefinition::Unpack(local_name, std::move(names), node->GetType()));
    } else {
        if (node_type != lang::Type::Singleton()) {
            node2unpack_[node] = {local_name};
        }
    }

    return std::nullopt;
}

std::optional<TranslationError> HaskellFuncTranslationRequest::AddBranchesForNode(
    const CPNode* node, const common::Cond& node_cond, const std::string& local_name) {
    auto maybe_branchers = MakeHaskellBranchers(node, local_name);
    if (!maybe_branchers.has_value()) {
        return maybe_branchers.error();
    }

    auto branch_conds = expr_builder_.AddBranches(node_cond, std::move(maybe_branchers.value()));

    for (const auto [out_pin, cond] : std::views::zip(node->OutPins(), branch_conds)) {
        pin2cond_[&out_pin] = cond;
    }

    return std::nullopt;
}

std::string HaskellFuncTranslationRequest::NameNode(const CPNode* node) {
    if (node->GetName().empty()) {
        std::string name = "catVar" + std::to_string(var_idx_++);
        node2local_name_[node] = name;
        return name;
    }

    if (node == &root_) {
        node2local_name_[node] = "catArgs";
        return "catArgs";
    }

    node2local_name_[node] = node->GetName();
    return node->GetName();
}

std::string HaskellFuncTranslationRequest::NewVarName() {
    return "catVar" + std::to_string(var_idx_++);
}

bool HaskellFuncTranslationRequest::HasNonTrivialBrancher(const CPNode* node) const {
    auto& out_pins = node->OutPins();

    if (out_pins.size() > 1) {
        return true;
    }

    if (out_pins.empty()) {
        return false;
    }

    const CPBrancher& brancher = out_pins.front().GetBrancher();

    return std::visit(util::Overloaded{
                          [](const lang::Pattern& pattern) -> bool {
                              return !pattern.Holds<lang::AnyPattern>();
                          },
                          [](const lang::Guard&) -> bool {
                              return true;
                          },
                      },
                      brancher);
}

bool HaskellFuncTranslationRequest::IsBrancherNode(const CPNode* node) const {
    return node->OutPins().size() > 1;
}

bool HaskellFuncTranslationRequest::IsIntersectionNode(const CPNode* node) const {
    if (node->IncomingArrows().empty()) {
        return false;
    }

    for (const CPArrow* arrow : node->IncomingArrows()) {
        if (!arrow->GetMorphism()->Holds<lang::PositionMorphism>()) {
            return false;
        }
    }

    return true;
}

bool HaskellFuncTranslationRequest::IsNormalNode(const CPNode* node) const {
    for (const CPArrow* arrow : node->IncomingArrows()) {
        if (arrow->GetMorphism()->Holds<lang::PositionMorphism>()) {
            return false;
        }
    }

    return true;
}

TranslationResult<std::vector<HaskellBrancher>> HaskellFuncTranslationRequest::MakeHaskellBranchers(
    const CPNode* node, const std::string& local_name) {
    std::vector<HaskellBrancher> branchers;

    for (const CPOutPin& out_pin : node->OutPins()) {
        const CPBrancher& brancher = out_pin.GetBrancher();
        auto maybe_hs_brancher =
            HaskellBrancher::Make(local_name, node2deduced_type_[node], brancher);
        if (!maybe_hs_brancher.has_value()) {
            return std::unexpected(std::move(maybe_hs_brancher.error()));
        }

        branchers.push_back(std::move(maybe_hs_brancher.value()));
    }

    return branchers;
}

TranslationResult<HaskellDefinition> HaskellFuncTranslationRequest::MakeHaskellDefinitionForArrow(
    const CPArrow* arrow, const std::string& local_name) {
    const CPNode& dst_node = arrow->TargetNode();

    lang::Type dst_type = node2deduced_type_[&dst_node];

    std::map<size_t, lang::MorphismPtr> mapping;

    const CPNode* src_node = &arrow->SourcePin().GetNode();
    lang::Type src_type = node2deduced_type_[src_node];
    std::vector<lang::Type> src_types(src_type.GetComponents());

    for (const auto [i, name] : util::Enumerate(node2unpack_[src_node])) {
        mapping[i] = lang::Morphism::CommonWithType(name, src_types[i]);
    }

    auto full_morphism = std::invoke([&]() -> lang::MorphismPtr {
        if (mapping.empty()) {
            return arrow->GetMorphism();
        }

        return lang::Morphism::Binded(arrow->GetMorphism(), std::move(mapping));
    });

    return HaskellDefinition::Normal(local_name, {}, dst_type,
                                     HaskellExpr::Simple(dst_type, full_morphism->ToString()));
}

}  // namespace komaru::translate::hs
