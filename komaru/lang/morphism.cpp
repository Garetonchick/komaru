#include "morphism.hpp"

#include <cassert>
#include <format>

#include <komaru/util/std_extensions.hpp>

namespace komaru::lang {

CommonMorphism::CommonMorphism(std::string name, Type source, Type target)
    : name_(std::move(name)),
      source_(source),
      target_(target) {
}

std::string CommonMorphism::ToString() const {
    return name_;
}

Type CommonMorphism::GetSource() const {
    return source_;
}

Type CommonMorphism::GetTarget() const {
    return target_;
}

Type CommonMorphism::GetType() const {
    return Type::Function(source_, target_);
}

size_t CommonMorphism::GetParamNum() const {
    return GetType().GetParamNum();
}

bool CommonMorphism::IsValue() const {
    return GetType().IsValueType();
}

bool CommonMorphism::ShouldBeShielded() const {
    return IsOperatorName(name_) || GetParamNum() > 0;
}

bool CommonMorphism::IsOperator() const {
    return IsOperatorName(name_);
}

const std::string& CommonMorphism::GetName() const {
    return name_;
}

PositionMorphism::PositionMorphism(size_t pos)
    : pos_(pos) {
}

std::string PositionMorphism::ToString() const {
    if (IsNonePosition()) {
        return "$";
    }
    return std::format("${}", pos_);
}

Type PositionMorphism::GetSource() const {
    return Type::Auto();
}

Type PositionMorphism::GetTarget() const {
    return Type::Auto();
}

Type PositionMorphism::GetType() const {
    return Type::Function(Type::Auto(), Type::Auto());
}

size_t PositionMorphism::GetParamNum() const {
    return 1;
}

bool PositionMorphism::IsValue() const {
    return false;
}

bool PositionMorphism::ShouldBeShielded() const {
    return false;
}

bool PositionMorphism::IsOperator() const {
    return false;
}

size_t PositionMorphism::GetPosition() const {
    return pos_;
}

bool PositionMorphism::IsNonePosition() const {
    return pos_ == std::numeric_limits<size_t>::max();
}

BindedMorphism::BindedMorphism(MorphismPtr morphism, std::map<size_t, MorphismPtr> mapping)
    : morphism_(std::move(morphism)),
      mapping_(std::move(mapping)),
      source_(Type::Auto()),
      target_(Type::Auto()) {
    size_t param_num = morphism_->GetType().GetParamNum();
    std::vector<Type> new_types;

    // Cleanup extra mappings
    auto it = mapping_.lower_bound(param_num - 1);
    while (it != mapping_.end()) {
        it = mapping_.erase(it);
    }

    assert(!mapping_.empty());

    std::map<size_t, Type> type_mapping;
    for (const auto& [pos, morphism] : mapping_) {
        type_mapping[pos] = morphism->GetType();
    }

    Type deduced_type = DeduceTypes(morphism_->GetType(), type_mapping);
    auto types = deduced_type.FlattenFunction();

    for (size_t i = 0; i + 1 < types.size(); ++i) {
        if (!mapping_.contains(i)) {
            new_types.push_back(types[i]);
        }
    }

    new_types.push_back(types.back());

    const FunctionType& new_type = Type::FunctionChain(new_types).GetVariant<FunctionType>();
    source_ = new_type.Source();
    target_ = new_type.Target();
}

std::string BindedMorphism::ToString() const {
    if (IsOperatorName(morphism_->ToString())) {
        return ToStringAsOperator();
    }

    std::string res = ToStringShielded(*morphism_);

    size_t last_idx = mapping_.rbegin()->first;

    for (size_t i = 0; i <= last_idx; ++i) {
        res += " ";

        auto it = mapping_.find(i);
        if (it != mapping_.end()) {
            res += ToStringShielded(*it->second);
        } else {
            res += "_";
        }
    }

    return res;
}

Type BindedMorphism::GetSource() const {
    return source_;
}

Type BindedMorphism::GetTarget() const {
    return target_;
}

Type BindedMorphism::GetType() const {
    return Type::Function(source_, target_);
}

size_t BindedMorphism::GetParamNum() const {
    return GetType().GetParamNum();
}

bool BindedMorphism::IsValue() const {
    return GetType().IsValueType();
}

bool BindedMorphism::ShouldBeShielded() const {
    return true;
}

bool BindedMorphism::IsOperator() const {
    return false;
}

const MorphismPtr& BindedMorphism::GetUnderlyingMorphism() const {
    return morphism_;
}

const std::map<size_t, MorphismPtr>& BindedMorphism::GetMapping() const {
    return mapping_;
}

std::string BindedMorphism::ToStringAsOperator() const {
    std::string res;

    auto it = mapping_.find(0);

    if (it != mapping_.end()) {
        res += ToStringShielded(*it->second);
        res += " ";
    }

    res += morphism_->ToString();

    it = mapping_.find(1);
    if (it != mapping_.end()) {
        res += " ";
        res += it->second->ToString();
    }

    return res;
}

LiteralMorphism::LiteralMorphism(Literal literal)
    : literal_(std::move(literal)) {
}

std::string LiteralMorphism::ToString() const {
    return literal_.ToString();
}

Type LiteralMorphism::GetSource() const {
    return Type::Singleton();
}

Type LiteralMorphism::GetTarget() const {
    return literal_.GetType();
}

Type LiteralMorphism::GetType() const {
    return Type::Function(Type::Singleton(), literal_.GetType());
}

size_t LiteralMorphism::GetParamNum() const {
    return 0;
}

bool LiteralMorphism::IsValue() const {
    return true;
}

bool LiteralMorphism::ShouldBeShielded() const {
    return false;
}

bool LiteralMorphism::IsOperator() const {
    return false;
}

const Literal& LiteralMorphism::GetLiteral() const {
    return literal_;
}

MorphismPtr Morphism::Common(std::string name, Type source, Type target) {
    return std::make_shared<Morphism>(PrivateDummy{},
                                      CommonMorphism(std::move(name), source, target));
}

MorphismPtr Morphism::CommonWithType(std::string name, Type type) {
    return type.Visit(
        util::Overloaded{[&](const FunctionType& t) -> MorphismPtr {
                             return Morphism::Common(std::move(name), t.Source(), t.Target());
                         },
                         [&](const auto&) -> MorphismPtr {
                             return Morphism::Common(std::move(name), Type::Singleton(), type);
                         }});
}

MorphismPtr Morphism::Value(std::string name, Type type) {
    return Common(std::move(name), Type::Singleton(), type);
}

MorphismPtr Morphism::ChainFunction(std::string name, const std::vector<Type>& types) {
    return Morphism::CommonWithType(std::move(name), Type::FunctionChain(types));
}

MorphismPtr Morphism::Position(size_t pos) {
    return std::make_shared<Morphism>(PrivateDummy{}, PositionMorphism(pos));
}

MorphismPtr Morphism::NonePosition() {
    return Position(std::numeric_limits<size_t>::max());
}

MorphismPtr Morphism::Binded(MorphismPtr morphism, std::map<size_t, MorphismPtr> mapping) {
    return std::make_shared<Morphism>(PrivateDummy{},
                                      BindedMorphism(std::move(morphism), std::move(mapping)));
}

MorphismPtr Morphism::Literal(class Literal literal) {
    return std::make_shared<Morphism>(PrivateDummy{}, LiteralMorphism(std::move(literal)));
}

MorphismPtr Morphism::Plus() {
    return Morphism::ChainFunction(
        "+", std::vector<Type>{Type::Var("a"), Type::Var("a"), Type::Var("a")});
}

MorphismPtr Morphism::Minus() {
    return Morphism::ChainFunction(
        "-", std::vector<Type>{Type::Var("a"), Type::Var("a"), Type::Var("a")});
}

MorphismPtr Morphism::Multiply() {
    return Morphism::ChainFunction(
        "*", std::vector<Type>{Type::Var("a"), Type::Var("a"), Type::Var("a")});
}

MorphismPtr Morphism::Greater() {
    return Morphism::ChainFunction(">",
                                   std::vector<Type>{Type::Var("a"), Type::Var("a"), Type::Bool()});
}

MorphismPtr Morphism::Less() {
    return Morphism::ChainFunction("<",
                                   std::vector<Type>{Type::Var("a"), Type::Var("a"), Type::Bool()});
}

MorphismPtr Morphism::GreaterEq() {
    return Morphism::ChainFunction(">=",
                                   std::vector<Type>{Type::Var("a"), Type::Var("a"), Type::Bool()});
}

MorphismPtr Morphism::LessEq() {
    return Morphism::ChainFunction("<=",
                                   std::vector<Type>{Type::Var("a"), Type::Var("a"), Type::Bool()});
}

MorphismPtr Morphism::Identity() {
    return Morphism::Common("id", Type::Var("a"), Type::Var("a"));
}

MorphismPtr Morphism::True() {
    return Morphism::Common("True", Type::Singleton(), Type::Bool());
}

MorphismPtr Morphism::False() {
    return Morphism::Common("False", Type::Singleton(), Type::Bool());
}

std::string Morphism::ToString() const {
    return Visit([](const auto& morphism) -> std::string {
        return morphism.ToString();
    });
}

Type Morphism::GetSource() const {
    return Visit([](const auto& morphism) {
        return morphism.GetSource();
    });
}

Type Morphism::GetTarget() const {
    return Visit([](const auto& morphism) {
        return morphism.GetTarget();
    });
}

Type Morphism::GetType() const {
    return Visit([](const auto& morphism) {
        return morphism.GetType();
    });
}

size_t Morphism::GetParamNum() const {
    return GetType().GetParamNum();
}

bool Morphism::IsValue() const {
    return GetType().IsValueType();
}

bool Morphism::ShouldBeShielded() const {
    return Visit([](const auto& morphism) -> bool {
        return morphism.ShouldBeShielded();
    });
}

bool Morphism::IsOperator() const {
    return Visit([](const auto& morphism) -> bool {
        return morphism.IsOperator();
    });
}

const Morphism::Variant* Morphism::GetVariantPointer() const {
    return &morphism_;
}

bool IsOperatorName(const std::string& name) {
    return !name.empty() && !std::isalnum(name.front());
}

bool IsFunctionName(const std::string& name) {
    return !name.empty() && std::isalpha(name.front());
}

bool IsConstructorName(const std::string& name) {
    return !name.empty() && std::isalpha(name.front()) && std::isupper(name.front());
}

std::string ToStringShielded(const Morphism& morphism) {
    if (morphism.ShouldBeShielded()) {
        return std::format("({})", morphism.ToString());
    }
    return morphism.ToString();
}

}  // namespace komaru::lang
