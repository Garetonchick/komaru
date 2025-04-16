#include "morphism.hpp"

#include <cassert>
#include <format>

namespace komaru::lang {

// TODO: better
static std::string_view TagToName(MorphismTag tag) {
    switch (tag) {
        case MorphismTag::Id:
            return "id";
        case MorphismTag::Plus:
            return "+";
        case MorphismTag::Minus:
            return "-";
        case MorphismTag::Multiply:
            return "*";
        case MorphismTag::Less:
            return "<";
        case MorphismTag::LessEq:
            return "<=";
        case MorphismTag::Greater:
            return ">";
        case MorphismTag::GreaterEq:
            return ">=";
        case MorphismTag::DebugInt:
            return "debug_int";
        case MorphismTag::Compound:
            return "ERROR (Compound)";
        case MorphismTag::Value:
            return "ERROR (Value)";
        case MorphismTag::Position:
            return "ERROR (Position)";
        case MorphismTag::Binded:
            return "ERROR (Binded)";
        case MorphismTag::Name:
            return "ERROR (Name)";
    }

    throw std::logic_error("unknown morphism tag");
}

BuiltinMorphism::BuiltinMorphism(MorphismTag tag, Type source, Type target)
    : name_(TagToName(tag)),
      tag_(tag),
      source_type_(source),
      target_type_(target) {
}

const std::string& BuiltinMorphism::GetName() const {
    return name_;
}

Type BuiltinMorphism::GetSource() const {
    return source_type_;
}

Type BuiltinMorphism::GetTarget() const {
    return target_type_;
}

MorphismTag BuiltinMorphism::GetTag() const {
    return tag_;
}

CompoundMorphism::CompoundMorphism(std::string name, std::vector<MorphismPtr> morphisms)
    : name_(std::move(name)),
      morphisms_(std::move(morphisms)) {
}

const std::string& CompoundMorphism::GetName() const {
    return name_;
}

Type CompoundMorphism::GetSource() const {
    return morphisms_.front()->GetSource();
}

Type CompoundMorphism::GetTarget() const {
    return morphisms_.back()->GetTarget();
}

MorphismTag CompoundMorphism::GetTag() const {
    return MorphismTag::Compound;
}

const std::vector<MorphismPtr>& CompoundMorphism::GetMorphisms() const {
    return morphisms_;
}

ValueMorphism::ValueMorphism(std::string name, Value value, bool strict)
    : name_(std::move(name)),
      value_(std::move(value)),
      strict_(strict) {
}

const std::string& ValueMorphism::GetName() const {
    return name_;
}

Type ValueMorphism::GetSource() const {
    return strict_ ? Type::Singleton() : Type::Auto();
}

Type ValueMorphism::GetTarget() const {
    return value_.GetType();
}

MorphismTag ValueMorphism::GetTag() const {
    return MorphismTag::Value;
}

const Value& ValueMorphism::GetValue() const {
    return value_;
}

MorphismPtr ValueMorphism::Unrestricted() const {
    return Morphism::WithValue(name_, value_, false);
}

PositionMorphism::PositionMorphism(size_t pos)
    : pos_(pos),
      name_(std::format("${}", pos_)) {
}

const std::string& PositionMorphism::GetName() const {
    return name_;
}

Type PositionMorphism::GetSource() const {
    return Type::Auto();
}

Type PositionMorphism::GetTarget() const {
    return Type::Auto();
}

MorphismTag PositionMorphism::GetTag() const {
    return MorphismTag::Position;
}

size_t PositionMorphism::GetPosition() const {
    return pos_;
}

bool PositionMorphism::IsNonePosition() const {
    return pos_ == std::numeric_limits<size_t>::max();
}

BindedMorphism::BindedMorphism(MorphismPtr morphism, std::map<size_t, Value> mapping)
    : morphism_(std::move(morphism)),
      mapping_(std::move(mapping)) {
}

const std::string& BindedMorphism::GetName() const {
    return morphism_->GetName();
}

Type BindedMorphism::GetSource() const {
    return morphism_->GetSource();
}

Type BindedMorphism::GetTarget() const {
    return morphism_->GetTarget();
}

MorphismTag BindedMorphism::GetTag() const {
    return MorphismTag::Binded;
}

const MorphismPtr& BindedMorphism::GetUnderlyingMorphism() const {
    return morphism_;
}

const std::map<size_t, Value>& BindedMorphism::GetMapping() const {
    return mapping_;
}

NameMorphism::NameMorphism(std::string name, Type source, Type target)
    : name_(std::move(name)),
      source_(source),
      target_(target) {
}

const std::string& NameMorphism::GetName() const {
    return name_;
}

Type NameMorphism::GetSource() const {
    return source_;
}

Type NameMorphism::GetTarget() const {
    return target_;
}

MorphismTag NameMorphism::GetTag() const {
    return MorphismTag::Name;
}

MorphismPtr Morphism::Builtin(MorphismTag tag, Type source, Type target) {
    return std::make_shared<Morphism>(PrivateDummy{}, BuiltinMorphism(tag, source, target));
}

MorphismPtr Morphism::Compound(std::string name, std::vector<MorphismPtr> morphisms) {
    if (!ValidateCompound(morphisms)) {
        throw std::runtime_error("Attempt to create invalid compound morphism");
    }
    return std::make_shared<Morphism>(PrivateDummy{},
                                      CompoundMorphism(std::move(name), std::move(morphisms)));
}

MorphismPtr Morphism::WithValue(std::string name, Value value, bool strict) {
    return std::make_shared<Morphism>(PrivateDummy{}, ValueMorphism(name, value, strict));
}

MorphismPtr Morphism::Position(size_t pos) {
    return std::make_shared<Morphism>(PrivateDummy{}, PositionMorphism(pos));
}

MorphismPtr Morphism::NonePosition() {
    return Position(std::numeric_limits<size_t>::max());
}

MorphismPtr Morphism::WithName(std::string name, Type source, Type target) {
    return std::make_shared<Morphism>(PrivateDummy{},
                                      NameMorphism(std::move(name), source, target));
}

const std::string& Morphism::GetName() const {
    return Visit([](const auto& morphism) -> const std::string& {
        return morphism.GetName();
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

MorphismTag Morphism::GetTag() const {
    return Visit([](const auto& morphism) {
        return morphism.GetTag();
    });
}

bool Morphism::ValidateCompound(const std::vector<MorphismPtr>& morphisms) {
    if (morphisms.empty()) {
        return false;
    }
    for (size_t i = 0; i + 1 < morphisms.size(); ++i) {
        if (morphisms[i]->GetTarget() != morphisms[i + 1]->GetSource()) {
            return false;
        }
    }
    return true;
}

const Morphism::Variant* Morphism::GetVariantPointer() const {
    return &morphism_;
}

MorphismPtr BindMorphism(MorphismPtr morphism, std::map<size_t, Value> mapping) {
    return std::make_shared<Morphism>(Morphism::PrivateDummy{},
                                      BindedMorphism(std::move(morphism), std::move(mapping)));
}

}  // namespace komaru::lang
