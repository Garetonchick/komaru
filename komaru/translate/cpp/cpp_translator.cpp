#include "cpp_translator.hpp"

#include <komaru/translate/cpp/cpp_program.hpp>
#include <komaru/translate/cpp/cpp_function_builder.hpp>
#include <komaru/translate/cpp/cpp_value.hpp>
#include <komaru/translate/cpp/cpp_types.hpp>
#include <komaru/translate/cpp/cpp_body_builder.hpp>
#include <komaru/util/std_extensions.hpp>
#include <komaru/util/string.hpp>

#include <cassert>
#include <algorithm>
#include <format>

namespace komaru::translate::cpp {

const std::unordered_map<std::string, std::string> CppTranslator::kNameConv = {
    {"+", "Plus"},
    {"-", "Minus"},
    {"*", "Mul"},
};

TranslationResult<std::unique_ptr<IProgram>> CppTranslator::Translate(
    const lang::CatProgram& cat_prog) {
    Reset();

    const CPNode* main_node = nullptr;

    for (const auto& node : cat_prog.GetNodes()) {
        if (node.GetName() == "main") {
            if (main_node) {
                return MakeTranslationError("found multiple main functions");
            }
            main_node = &node;
        }
    }

    if (!main_node) {
        return MakeTranslationError("main function not found");
    }

    if (!main_node->IncomingArrows().empty()) {
        return MakeTranslationError("main node must not have incoming pins");
    }

    if (auto maybe_err = CalcRoots(cat_prog)) {
        return std::unexpected(maybe_err.value());
    }

    auto maybe_roots = DiscoverFunctions(cat_prog);
    if (!maybe_roots.has_value()) {
        return std::unexpected(maybe_roots.error());
    }
    auto roots = maybe_roots.value();

    for (const auto* root : roots) {
        auto func_or_err = TranslateMorphismGraph(root);

        if (!func_or_err.has_value()) {
            return std::unexpected(std::move(func_or_err.error()));
        }

        builder_.AddFunction(std::move(func_or_err.value()));
    }

    auto main_cpp_func = CppFunctionBuilder()
                             .SetName("main")
                             .SetReturnType(lang::Type::Int())
                             .SetBody("std::cout << cat__main({}) << std::endl;")
                             .Extract();

    builder_.AddFunction(std::move(main_cpp_func));

    builder_.AddHeader("cstdint");
    builder_.AddHeader("tuple");
    builder_.AddHeader("variant");
    builder_.AddHeader("iostream");

    return builder_.ExtractProgram();
}

TranslationResult<CppFunction> CppTranslator::TranslateMorphismGraph(const CPNode* root) {
    local_name2type_.clear();

    auto make_node_name = [local_var_id = size_t(0)](const CPNode* node) mutable {
        if (!node->GetName().empty()) {
            return node->GetName();
        }
        return std::format("var__{}", local_var_id++);
    };

    CppBodyBuilder body_builder;
    std::queue<const CPNode*> normal_q;
    std::queue<const CPNode*> branch_q;

    normal_q.push(root);
    node2local_name_[root] = "cat__arg";

    while (!normal_q.empty() || !branch_q.empty()) {
        bool first_visit = true;
        const CPNode* node = nullptr;
        if (!normal_q.empty()) {
            node = normal_q.front();
            normal_q.pop();
        } else {
            node = branch_q.front();
            branch_q.pop();
            first_visit = false;
        }

        bool branch_node = node->OutPins().size() > 1;
        CppCond node_cond = CalcCppCondForNode(node);

        if (first_visit && node->OutPins().size() == 1) {
            pin2cond_[&node->OutPins().front()] = node_cond;
        }

        if (first_visit && node != root) {
            std::string local_name = make_node_name(node);
            node2local_name_[node] = local_name;
            if (!node->GetName().empty()) {
                local_name2type_.emplace(local_name, node->GetType());
            }
            AddStatementsForNode(body_builder, node_cond, node, local_name);
        } else if (!first_visit) {
            auto branch_conds = body_builder.AddBranches(node_cond, MakeBranchExprs(node));
            for (const auto [out_pin, cond] : std::views::zip(node->OutPins(), branch_conds)) {
                pin2cond_[&out_pin] = cond;
            }
        }

        if (branch_node && first_visit) {
            branch_q.push(node);
            continue;
        }

        for (const auto& out_pin : node->OutPins()) {
            for (const auto& arrow : out_pin.Arrows()) {
                const CPNode* target_node = &arrow.TargetNode();
                size_t n_incoming = target_node->IncomingArrows().size();
                auto [it, inserted] = node2views_.emplace(target_node, 1);
                bool inc = inserted;
                if (!inserted) {
                    if (it->second < n_incoming) {
                        ++it->second;
                        inc = true;
                    }
                }

                if (inc && it->second == n_incoming) {
                    normal_q.push(target_node);
                }
            }
        }
    }

    auto maybe_body = body_builder.Extract();
    if (!maybe_body.has_value()) {
        return std::unexpected(maybe_body.error());
    }

    std::string body = std::move(maybe_body.value());

    auto ret_type =
        global_name2type_.find(root->GetName())->second.GetVariant<lang::FunctionType>().Target();

    auto func_builder = CppFunctionBuilder()
                            .SetReturnType(ret_type)
                            .AddInputParameter(root->GetType(), "cat__arg")
                            .SetBody(body);

    if (root->GetName() == "main") {
        func_builder.SetName("cat__main");
    } else {
        func_builder.SetName(root->GetName());
    }

    return std::move(func_builder).Extract();
}

void CppTranslator::AddStatementsForNode(CppBodyBuilder& body_builder, const CppCond& node_cond,
                                         const CPNode* node, const std::string& local_name) {
    if (IsIntersectionNode(node)) {
        std::string expr = MakeExprForIntersectionNode(node);
        auto statement = MakeStatement(node->GetType(), local_name, expr);
        body_builder.AddStatement(node_cond, std::move(statement));

        if (node->OutPins().empty()) {
            body_builder.AddReturn(node_cond, std::format("return {}", local_name));
        }
        return;
    }

    for (const auto* arrow : node->IncomingArrows()) {
        std::string expr = MakeExprForArrow(arrow);
        auto statement = MakeStatement(node->GetType(), local_name, expr);
        body_builder.AddStatement(pin2cond_[&arrow->SourcePin()], std::move(statement));

        if (node->OutPins().empty()) {
            body_builder.AddReturn(node_cond, std::format("return {}", local_name));
        }
    }
}

bool CppTranslator::IsIntersectionNode(const CPNode* node) {
    if (node->IncomingArrows().empty()) {
        return false;
    }
    return node->IncomingArrows().front()->GetMorphism()->Holds<lang::PositionMorphism>();
}

std::string CppTranslator::MakeExprForIntersectionNode(const CPNode* node) {
    std::string expr = "std::make_tuple(";
    std::vector<std::pair<size_t, const CPNode*>> order;

    for (const auto* arrow : node->IncomingArrows()) {
        // TODO: Properly check if it really is PositionMorphism
        const auto& pos_morphism = arrow->GetMorphism()->GetVariant<lang::PositionMorphism>();

        if (!pos_morphism.IsNonePosition()) {
            order.emplace_back(pos_morphism.GetPosition(), &arrow->SourcePin().GetNode());
        }
    }

    if (order.empty()) {
        return "std::monostate{}";  // use this instead of an empty tuple
    }

    if (order.size() == 1) {
        return node2local_name_[order.front().second];
    }

    std::ranges::sort(order);

    for (const auto [i, p] : util::Enumerate(order)) {
        const auto [pos, node] = p;

        if (i != pos) {
            // TODO: return error instead throwing
            throw std::runtime_error("intersection node has invalid incoming position morphisms");
        }

        expr += node2local_name_[node];

        if (i + 1 != order.size()) {
            expr += ", ";
        }
    }

    expr += ")";

    return expr;
}

std::string CppTranslator::MakeExprForArrow(const CPArrow* arrow) {
    return MakeExprForMorphism(*arrow->GetMorphism(),
                               node2local_name_[&arrow->SourcePin().GetNode()]);
}

std::vector<std::string> CppTranslator::MakeBranchExprs(const CPNode* node) {
    std::vector<std::string> exprs;
    exprs.reserve(node->OutPins().size());

    std::string local_name = node2local_name_[node];

    for (const auto& out_pin : node->OutPins()) {
        exprs.emplace_back(MakeExprForBrancher(out_pin.GetBrancher(), local_name));
    }

    return exprs;
}

std::string CppTranslator::MakeExprForBrancher(const CPOutPin::Brancher& brancher,
                                               const std::string& arg_name) {
    return std::visit(util::Overloaded{[&, this](const lang::Guard& guard) {
                                           return MakeExprForGuard(guard, arg_name);
                                       },
                                       [&, this](const lang::Pattern& pattern) {
                                           return MakeExprForPattern(pattern, arg_name);
                                       }},
                      brancher);
}

std::string CppTranslator::MakeExprForGuard(const lang::Guard& guard, const std::string& arg_name) {
    return MakeExprForMorphism(guard.GetMorphism(), arg_name);
}

// TODO: Support name binding for patterns
std::string CppTranslator::MakeExprForPattern(const lang::Pattern& pattern,
                                              const std::string& arg_name) {
    return pattern.Visit([this, &arg_name](const auto& pattern_variant) {
        return MakeExprForPattern(pattern_variant, arg_name);
    });
}

std::string CppTranslator::MakeExprForPattern(const lang::AnyPattern&, const std::string&) {
    return "true";
}

std::string CppTranslator::MakeExprForPattern(const lang::ValuePattern& pattern,
                                              const std::string& arg_name) {
    auto cpp_value = ToCppValue(pattern.GetValue());
    return std::format("{} == {}", cpp_value, arg_name);
}

std::string CppTranslator::MakeExprForPattern(const lang::TuplePattern& pattern,
                                              const std::string& arg_name) {
    const auto& sub_patterns = pattern.GetPatterns();

    if (sub_patterns.empty()) {
        return std::format("{} == std::monostate{{}}", arg_name);
    }

    return pattern.GetPatterns() |
           std::views::transform([this, i = size_t(0), &arg_name](const auto& sub_pattern) mutable {
               std::string sub_arg_name = std::format("std::get<{}>({})", i, arg_name);
               ++i;
               return std::format("({})", MakeExprForPattern(sub_pattern, sub_arg_name));
           }) |
           util::JoinStrings("&&") | std::ranges::to<std::string>();
}

std::string CppTranslator::MakeExprForMorphism(const lang::Morphism& morphism,
                                               const std::string& arg_name) {
    return morphism.Visit(
        util::Overloaded{[](const lang::PositionMorphism&) -> std::string {
                             throw std::runtime_error(
                                 "position morphism can't be converted to an expression directly");
                         },
                         [this, &arg_name](const auto& inner_morphism) {
                             return MakeExprForMorphism(inner_morphism, arg_name);
                         }});
}

std::string CppTranslator::MakeExprForMorphism(const lang::CompoundMorphism&, const std::string&) {
    throw std::runtime_error("compound morphisms are unsupported for now");
}

std::string CppTranslator::MakeExprForMorphism(const lang::BuiltinMorphism& morphism,
                                               const std::string& arg_name) {
    auto make_binary_expr = [&arg_name](const std::string& op) {
        return std::format("std::get<0>({}) {} std::get<1>({})", arg_name, op, arg_name);
    };

    switch (morphism.GetTag()) {
        case lang::MorphismTag::Plus:
            return make_binary_expr("+");
        case lang::MorphismTag::Minus:
            return make_binary_expr("-");
        case lang::MorphismTag::Multiply:
            return make_binary_expr("*");
        case lang::MorphismTag::Less:
            return make_binary_expr("<");
        case lang::MorphismTag::LessEq:
            return make_binary_expr("<=");
        case lang::MorphismTag::Greater:
            return make_binary_expr(">");
        case lang::MorphismTag::GreaterEq:
            return make_binary_expr(">=");
        case lang::MorphismTag::Id: {
            return arg_name;
        }
        default:
            throw std::runtime_error("Unsupported builtin morphism for cpp translation");
    }
}

std::string CppTranslator::MakeExprForMorphism(const lang::ValueMorphism& morphism,
                                               const std::string&) {
    return ToCppValue(morphism.GetValue());
}

std::string CppTranslator::MakeExprForMorphism(const lang::BindedMorphism& morphism,
                                               const std::string& arg_name) {
    std::string binded = "std::make_tuple(";
    size_t arg_idx = 0;
    for (const auto& [idx, val] : morphism.GetMapping()) {
        if (idx != arg_idx) {
            if (arg_idx) {
                binded += ", ";
            }
            binded += arg_name + ", " + ToCppValue(val);
        } else {
            ++arg_idx;

            if (idx) {
                binded += ", ";
            }

            binded += ToCppValue(val);
        }
    }

    binded += ")";

    return MakeExprForMorphism(*morphism.GetUnderlyingMorphism(), binded);
}

std::string CppTranslator::MakeExprForMorphism(const lang::NameMorphism& morphism,
                                               const std::string& arg_name) {
    lang::Type type = std::invoke([&, this]() {
        {
            auto it = local_name2type_.find(morphism.GetName());
            if (it != local_name2type_.end()) {
                return it->second;
            }
        }
        auto it = global_name2type_.find(morphism.GetName());
        if (it != global_name2type_.end()) {
            return it->second;
        }

        throw std::runtime_error(std::format("named morphism {} not found", morphism.GetName()));
    });

    if (type.Holds<lang::FunctionType>()) {
        return std::format("{}({})", morphism.GetName(), arg_name);
    }

    // TODO: Properly check types compatibility
    return morphism.GetName();
}

std::string CppTranslator::MakeStatement(lang::Type type, const std::string& var_name,
                                         const std::string& expr) {
    CppType cpp_type = ToCppType(type);
    // TODO: process template vars
    return std::format("{} {} = {}", cpp_type.GetTypeStr(), var_name, expr);
}

CppCond CppTranslator::CalcCppCondForNode(const CPNode* node) {
    CppCond node_cond;

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

std::optional<TranslationError> CppTranslator::CalcRoots(const lang::CatProgram& cat_program) {
    for (const CPNode& node : cat_program.GetNodes()) {
        if (!GetRootOrCalcIt(&node)) {
            return TranslationError("some functions have multiple root nodes");
        }
    }

    return std::nullopt;
}

std::optional<const CppTranslator::CPNode*> CppTranslator::GetRootOrCalcIt(const CPNode* node) {
    auto it = node2root_.find(node);
    if (it != node2root_.end()) {
        return it->second;
    }

    const auto& in_nodes = node->IncomingArrows() | std::views::transform([](CPArrow* arrow) {
                               return &arrow->SourcePin().GetNode();
                           });

    if (in_nodes.empty()) {
        node2root_.emplace(node, node);
        return node;
    }

    const CPNode* root = nullptr;

    for (const CPNode* in_node : in_nodes) {
        if (auto in_root = GetRootOrCalcIt(in_node)) {
            if (root && root != in_root.value()) {
                return std::nullopt;
            }
            root = in_root.value();
        } else {
            return std::nullopt;
        }
    }

    node2root_.emplace(node, root);
    return root;
}

const CppTranslator::CPNode* CppTranslator::GetRoot(const CPNode* node) {
    return node2root_.at(node);
}

TranslationResult<std::vector<const CppTranslator::CPNode*>> CppTranslator::DiscoverFunctions(
    const lang::CatProgram& cat_prog) {
    std::unordered_map<const CPNode*, lang::Type> root2ret_type;

    for (const auto& node : cat_prog.GetNodes()) {
        if (!node.OutPins().empty()) {
            continue;
        }

        auto root = GetRoot(&node);

        auto [it, inserted] = root2ret_type.emplace(root, node.GetType());

        if (inserted) {
            continue;
        }

        if (it->second != node.GetType()) {
            return MakeTranslationError(std::format("found 2 conflicting return types {} and {}",
                                                    it->second.GetName(),
                                                    node.GetType().GetName()));
        }
    }

    for (const auto& [node, ret_type] : root2ret_type) {
        if (node->GetName().empty()) {
            return MakeTranslationError("found root node with no name");
        }

        global_name2type_.emplace(node->GetName(), lang::Type::Function(node->GetType(), ret_type));
    }

    return root2ret_type | std::views::keys | std::ranges::to<std::vector<const CPNode*>>();
}

void CppTranslator::Reset() {
    this->~CppTranslator();
    new (this) CppTranslator();
}

}  // namespace komaru::translate::cpp
