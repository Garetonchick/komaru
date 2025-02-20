#include "morphism.hpp"

#include <cassert>

namespace komaru::lang {

// TODO: better
static std::string_view TagToName(MorphismTag tag) {
    switch(tag) {
        case MorphismTag::Id:
            return "id";
        case MorphismTag::Plus:
            return "+";
        case MorphismTag::Minus:
            return "-";
        case MorphismTag::Multiply:
            return "*";
        case MorphismTag::DebugInt:
            return "debug_int";
        case MorphismTag::Compound:
            return "ERROR (Compound)";
        case MorphismTag::Value:
            return "ERROR (Value)";
    }

    throw std::logic_error("unknown morphism tag");
}

BuiltinMorphism::BuiltinMorphism(MorphismTag tag, Type source, Type target)
    : name_(TagToName(tag))
    , tag_(tag)
    , source_type_(source)
    , target_type_(target) {
}

std::string_view BuiltinMorphism::GetName() const {
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
    : name_(std::move(name)), morphisms_(std::move(morphisms)) {
}

std::string_view CompoundMorphism::GetName() const {
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

ValueMorphism::ValueMorphism(std::string name, Value value)
    : name_(std::move(name))
    , value_(std::move(value)) {
}

std::string_view ValueMorphism::GetName() const {
    return name_;
}

Type ValueMorphism::GetSource() const {
    return Type::FromTag(TypeTag::Singleton);
}

Type ValueMorphism::GetTarget() const {
    return value_.GetType();
}

MorphismTag ValueMorphism::GetTag() const {
    return MorphismTag::Value;
}

MorphismPtr Morphism::Builtin(MorphismTag tag, Type source, Type target) {
    return std::make_shared<Morphism>(
        PrivateDummy{}, BuiltinMorphism(tag, source, target)
    );
}

MorphismPtr Morphism::Compound(std::string name, std::vector<MorphismPtr> morphisms) {
    if(!ValidateCompound(morphisms)) {
        throw std::runtime_error("Attempt to create invalid compound morphism");
    }
    return std::make_shared<Morphism>(
        PrivateDummy{}, CompoundMorphism(std::move(name), std::move(morphisms))
    );
}

MorphismPtr Morphism::WithValue(std::string name, Value value) {
    return std::make_shared<Morphism>(
        PrivateDummy{}, ValueMorphism(name, value)
    );
}

std::string_view Morphism::GetName() const {
    return Visit([](const auto& morphism) {
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
    if(morphisms.empty()) {
        return false;
    }
    for(size_t i = 0; i + 1 < morphisms.size(); ++i) {
        if(morphisms[i]->GetTarget() != morphisms[i + 1]->GetSource()) {
            return false;
        }
    }
    return true;
}

const Morphism::Variant* Morphism::GetVariantPointer() const {
    return &morphism_;
}

}
