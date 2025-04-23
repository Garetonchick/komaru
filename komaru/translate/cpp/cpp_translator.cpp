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
#include <print>

namespace komaru::translate::cpp {

const std::unordered_map<std::string, std::string> CppTranslator::kNameConv = {
    {"read", "Read"},
    {"print", "Print"},
    {"liftM2", "LiftM2"},
    {">>=", "Bind"},
};

const std::unordered_set<std::string> CppTranslator::kDeductionSet = {"read"};

CppTranslator::CppTranslator(const std::filesystem::path& catlib_dir)
    : catlib_dir_(std::filesystem::canonical(catlib_dir)) {
}

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

    LoadCatlib();

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

    auto main_cpp_func_builder = CppFunctionBuilder()
                                     .SetName("main")
                                     .SetReturnType(lang::Type::Int())
                                     .SetBody("std::cout << cat__main({}) << std::endl;");

    auto cat_main_it = global_name2type_.find("main");

    if (cat_main_it != global_name2type_.end() &&
        cat_main_it->second.GetVariant<lang::FunctionType>().Target() ==
            lang::Type::Parameterized("IO", {lang::Type::Singleton()})) {
        main_cpp_func_builder.SetBody("cat__main({}).Run();");
    }

    builder_.AddFunction(std::move(main_cpp_func_builder).Extract());

    builder_.AddHeader("cstdint");
    builder_.AddHeader("tuple");
    builder_.AddHeader("variant");
    builder_.AddHeader("iostream");
    builder_.AddHeader("catlib.hpp");

    builder_.AddIncludeDir(catlib_dir_);

    return builder_.ExtractProgram();
}

void CppTranslator::LoadCatlib() {
    auto at = lang::Type::Generic("a");
    auto bt = lang::Type::Generic("b");
    auto ct = lang::Type::Generic("c");
    auto io_a = lang::Type::Parameterized("IO", {at});
    auto io_b = lang::Type::Parameterized("IO", {bt});
    auto io_c = lang::Type::Parameterized("IO", {ct});
    auto io_s = lang::Type::Parameterized("IO", {lang::Type::Singleton()});

    global_name2type_.emplace(
        "read", lang::Type::Function(lang::Type::Singleton(),
                                     lang::Type::Parameterized("IO", {lang::Type::Auto()})));
    global_name2type_.emplace(
        "liftM2", lang::Type::Function(lang::Type::Function(at * bt, ct) * io_a * io_b, io_c));
    global_name2type_.emplace(">>=",
                              lang::Type::Function(io_a * lang::Type::Function(at, io_b), io_b));
    global_name2type_.emplace("print", lang::Type::Function(lang::Type::Auto(), io_s));
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
        std::string expr = MakeExprForIntersectionNode(node).AsWholeExpr();
        auto statement = MakeStatement(node->GetType(), local_name, expr);
        body_builder.AddStatement(node_cond, std::move(statement));

        if (node->OutPins().empty()) {
            body_builder.AddReturn(node_cond, std::format("return {}", local_name));
        }
        return;
    }

    for (const auto* arrow : node->IncomingArrows()) {
        std::string expr = MakeExprForArrow(arrow).AsWholeExpr();
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

CppExpr CppTranslator::MakeExprForIntersectionNode(const CPNode* node) {
    std::vector<std::pair<size_t, const CPNode*>> order;

    for (const auto* arrow : node->IncomingArrows()) {
        // TODO: Properly check if it really is PositionMorphism
        const auto& pos_morphism = arrow->GetMorphism()->GetVariant<lang::PositionMorphism>();

        if (!pos_morphism.IsNonePosition()) {
            order.emplace_back(pos_morphism.GetPosition(), &arrow->SourcePin().GetNode());
        }
    }

    if (order.empty()) {
        return CppExpr("std::monostate{}", 1);  // use this instead of an empty tuple
    }

    if (order.size() == 1) {
        return CppExpr(node2local_name_[order.front().second],
                       order.front().second->GetType().NumComponents());
    }

    std::ranges::sort(order);

    std::vector<std::string> exprs;

    for (const auto [i, p] : util::Enumerate(order)) {
        const auto [pos, node] = p;

        if (i != pos) {
            // TODO: return error instead throwing
            throw std::runtime_error("intersection node has invalid incoming position morphisms");
        }

        exprs.push_back(node2local_name_[node]);
    }

    return CppExpr(exprs);
}

CppExpr CppTranslator::MakeExprForArrow(const CPArrow* arrow) {
    return MakeExprForMorphism(*arrow->GetMorphism(),
                               CppExpr(node2local_name_[&arrow->SourcePin().GetNode()],
                                       arrow->SourcePin().GetNode().GetType().NumComponents()),
                               arrow->TargetNode().GetType());
}

std::vector<std::string> CppTranslator::MakeBranchExprs(const CPNode* node) {
    std::vector<std::string> exprs;
    exprs.reserve(node->OutPins().size());

    std::string local_name = node2local_name_[node];

    for (const auto& out_pin : node->OutPins()) {
        exprs.emplace_back(MakeExprForBrancher(out_pin.GetBrancher(),
                                               CppExpr(local_name, node->GetType().NumComponents()))
                               .AsWholeExpr());
    }

    return exprs;
}

CppExpr CppTranslator::MakeExprForBrancher(const CPOutPin::Brancher& brancher,
                                           const CppExpr& in_expr) {
    return std::visit(util::Overloaded{[&, this](const lang::Guard& guard) {
                                           return MakeExprForGuard(guard, in_expr);
                                       },
                                       [&, this](const lang::Pattern& pattern) {
                                           return MakeExprForPattern(pattern, in_expr);
                                       }},
                      brancher);
}

CppExpr CppTranslator::MakeExprForGuard(const lang::Guard& guard, const CppExpr& in_expr) {
    return MakeExprForMorphism(guard.GetMorphism(), in_expr, lang::Type::Bool());
}

// TODO: Support name binding for patterns
CppExpr CppTranslator::MakeExprForPattern(const lang::Pattern& pattern, const CppExpr& in_expr) {
    return pattern.Visit([this, &in_expr](const auto& pattern_variant) {
        return MakeExprForPattern(pattern_variant, in_expr);
    });
}

CppExpr CppTranslator::MakeExprForPattern(const lang::AnyPattern&, const CppExpr&) {
    return CppExpr("true", 1);
}

CppExpr CppTranslator::MakeExprForPattern(const lang::ValuePattern& pattern,
                                          const CppExpr& in_expr) {
    auto cpp_value = ToCppValue(pattern.GetValue());
    return CppExpr(std::format("{} == {}", cpp_value, in_expr.AsWholeExpr()), 1);
}

CppExpr CppTranslator::MakeExprForPattern(const lang::TuplePattern& pattern,
                                          const CppExpr& in_expr) {
    const auto& sub_patterns = pattern.GetPatterns();

    if (sub_patterns.empty()) {
        return CppExpr(std::format("{} == std::monostate{{}}", in_expr.AsWholeExpr()), 1);
    }
    std::string expr;

    for (const auto& [i, sub_pattern] : util::Enumerate(pattern.GetPatterns())) {
        std::string sub_arg_name = std::format("std::get<{}>({})", i, in_expr.AsWholeExpr());
        expr += MakeExprForPattern(sub_pattern, CppExpr(sub_arg_name, 1)).AsWholeExpr();

        if (i + 1 != pattern.GetPatterns().size()) {
            expr += " && ";
        }
    }

    return CppExpr(expr, 1);
}

CppExpr CppTranslator::MakeExprForMorphism(const lang::Morphism& morphism, const CppExpr& in_expr,
                                           lang::Type out_type) {
    return morphism.Visit(
        util::Overloaded{[](const lang::PositionMorphism&) -> CppExpr {
                             throw std::runtime_error(
                                 "position morphism can't be converted to an expression directly");
                         },
                         [this, &in_expr, &out_type](const auto& inner_morphism) {
                             return MakeExprForMorphism(inner_morphism, in_expr, out_type);
                         }});
}

CppExpr CppTranslator::MakeExprForMorphism(const lang::CompoundMorphism&, const CppExpr&,
                                           lang::Type) {
    throw std::runtime_error("compound morphisms are unsupported for now");
}

CppExpr CppTranslator::MakeExprForMorphism(const lang::BuiltinMorphism& morphism,
                                           const CppExpr& in_expr, lang::Type) {
    if (in_expr.NumSubexprs() == 0) {
        switch (morphism.GetTag()) {
            case lang::MorphismTag::Plus:
                return CppExpr({"Plus"});
            case lang::MorphismTag::Minus:
                return CppExpr({"Minus"});
            case lang::MorphismTag::Multiply:
                return CppExpr({"Multiply"});
            case lang::MorphismTag::Less:
                return CppExpr({"Less"});
            case lang::MorphismTag::LessEq:
                return CppExpr({"LessEq"});
            case lang::MorphismTag::Greater:
                return CppExpr({"Greater"});
            case lang::MorphismTag::GreaterEq:
                return CppExpr({"GreaterEq"});
            case lang::MorphismTag::Id: {
                return CppExpr({"Id"});
            }
            default:
                throw std::runtime_error("Unsupported builtin morphism for cpp translation");
        }
    }

    auto make_binary_expr = [&in_expr](const std::string& op) {
        const auto& subexprs = in_expr.GetSubexprs();
        return CppExpr(std::format("{} {} {}", subexprs[0], op, subexprs[1]), 1);
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
            return in_expr;
        }
        default:
            throw std::runtime_error("Unsupported builtin morphism for cpp translation");
    }
}

CppExpr CppTranslator::MakeExprForMorphism(const lang::ValueMorphism& morphism, const CppExpr&,
                                           lang::Type) {
    return CppExpr(ToCppValue(morphism.GetValue()), 1);
}

CppExpr CppTranslator::MakeExprForMorphism(const lang::BindedMorphism& morphism,
                                           const CppExpr& in_expr, lang::Type) {
    std::vector<std::string> exprs;
    size_t n = morphism.GetUnderlyingMorphism()->GetSource().NumComponents();
    const auto& mapping = morphism.GetMapping();
    if (n < mapping.size()) {
        throw std::runtime_error("too much binded args");
    }
    size_t left = n - mapping.size();
    bool use_subexprs = left == in_expr.NumSubexprs();
    size_t subexpr_idx = 0;

    for (size_t idx = 0; idx < n; ++idx) {
        auto it = mapping.find(idx);
        if (it != mapping.end()) {
            exprs.push_back(
                MakeExprForMorphism(*it->second, CppExpr({}), lang::Type::Auto()).AsWholeExpr());
            continue;
        }

        if (use_subexprs) {
            exprs.push_back(in_expr.GetSubexprs()[subexpr_idx]);
        } else if (left == 1) {
            exprs.push_back(in_expr.AsWholeExpr());
        } else {
            exprs.push_back(std::format("std::get<{}>({})", subexpr_idx, in_expr.AsWholeExpr()));
        }
        ++subexpr_idx;
    }

    return MakeExprForMorphism(*morphism.GetUnderlyingMorphism(), CppExpr(std::move(exprs)),
                               lang::Type::Auto());
}

CppExpr CppTranslator::MakeExprForMorphism(const lang::NameMorphism& morphism,
                                           const CppExpr& in_expr, lang::Type out_type) {
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
        if (in_expr.NumSubexprs() == 0) {
            return CppExpr(ToCppName(morphism.GetName()), 1);
        }
        size_t n_args = type.GetVariant<lang::FunctionType>().Source().NumComponents();

        auto args_str =
            in_expr.Cook(n_args) | util::JoinStrings(", ") | std::ranges::to<std::string>();

        if (kDeductionSet.contains(morphism.GetName())) {
            args_str += std::format(", DeductionTag<{}>{{}}", ToCppType(out_type).GetTypeStr());
        }

        return CppExpr(std::format("{}({})", ToCppName(morphism.GetName()), args_str),
                       type.GetVariant<lang::FunctionType>().Target().NumComponents());
    }

    // TODO: Properly check types compatibility
    return CppExpr(ToCppName(morphism.GetName()), 1);
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

std::string CppTranslator::ToCppName(const std::string& name) {
    auto it = kNameConv.find(name);
    if (it != kNameConv.end()) {
        return it->second;
    }
    return name;
}

void CppTranslator::Reset() {
    auto catlib_dir = catlib_dir_;
    this->~CppTranslator();
    new (this) CppTranslator(catlib_dir);
}

}  // namespace komaru::translate::cpp
