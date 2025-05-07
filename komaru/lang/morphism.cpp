#include "morphism.hpp"

#include <cassert>
#include <format>

#include <komaru/util/std_extensions.hpp>

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

std::string BuiltinMorphism::ToString() const {
    return GetName();
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

std::string CompoundMorphism::ToString() const {
    return "compound UNIMPLEMENTED";
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

std::string ValueMorphism::ToString() const {
    return value_.ToString();
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

std::string PositionMorphism::ToString() const {
    if (IsNonePosition()) {
        return "$";
    }
    return std::format("${}", pos_);
}

size_t PositionMorphism::GetPosition() const {
    return pos_;
}

bool PositionMorphism::IsNonePosition() const {
    return pos_ == std::numeric_limits<size_t>::max();
}

BindedMorphism::BindedMorphism(MorphismPtr morphism, std::map<size_t, MorphismPtr> mapping)
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

std::string BindedMorphism::ToString() const {
    std::string mname = morphism_->ToString();
    if (IsComplex(*morphism_)) {
        mname = "(" + mname + ")";
    }

    auto to_wrapped_str = [&](const Morphism& m) {
        if (IsOperator(m)) {
            return "(" + m.ToString() + ")";
        }
        return m.ToString();
    };

    std::string res;

    if (IsOperator(*morphism_)) {
        auto it = mapping_.find(0);
        if (it != mapping_.end()) {
            res += to_wrapped_str(*it->second) + " ";
        }
        res += mname;
        it = mapping_.find(1);
        if (it != mapping_.end()) {
            res += " " + to_wrapped_str(*it->second);
        }
    } else {
        size_t n_comps = morphism_->GetSource().GetComponentsNum();
        res = mname;
        size_t n_mapped = 0;

        for (size_t i = 0; i < n_comps && n_mapped < mapping_.size(); ++i) {
            auto it = mapping_.find(i);
            if (it != mapping_.end()) {
                res += " " + to_wrapped_str(*it->second);
                ++n_mapped;
            } else {
                res += " _";
            }
        }
    }

    return res;
}

const MorphismPtr& BindedMorphism::GetUnderlyingMorphism() const {
    return morphism_;
}

const std::map<size_t, MorphismPtr>& BindedMorphism::GetMapping() const {
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

std::string NameMorphism::ToString() const {
    return name_;
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

std::string Morphism::ToString() const {
    return Visit([](const auto& morphism) {
        return morphism.ToString();
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

MorphismPtr BindMorphism(MorphismPtr morphism, std::map<size_t, MorphismPtr> mapping) {
    return std::make_shared<Morphism>(Morphism::PrivateDummy{},
                                      BindedMorphism(std::move(morphism), std::move(mapping)));
}

bool IsOperator(const Morphism& morphism) {
    return morphism.Visit(
        util::Overloaded{[](const BuiltinMorphism& m) -> bool {
                             return m.GetTag() != MorphismTag::Id;  // TODO: change later
                         },
                         [](const NameMorphism& m) -> bool {
                             return !m.GetName().empty() && !std::isalpha(m.GetName().front());
                         },
                         [](const auto&) -> bool {
                             return false;
                         }});
}

bool IsComplex(const Morphism& morphism) {
    return morphism.Holds<BindedMorphism>();
}

bool IsFunction(const Morphism& morphism) {
    return !morphism.Holds<ValueMorphism>();
}

}  // namespace komaru::lang
