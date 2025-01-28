#include "type.hpp"

#include <format>
#include <cassert>

namespace komaru::lang {

AtomType::AtomType(TypeTag tag)
    : tag_(tag) {
    switch (tag) {
        case TypeTag::Int:
        case TypeTag::Bool:
        case TypeTag::Char:
        case TypeTag::Float:
            return;
        default:
            throw std::logic_error(std::format(
                "Atomic type can't have tag {}", static_cast<int>(tag)
            ));
    }
}

std::string_view AtomType::GetName() const {
    switch (tag_) {
        case TypeTag::Int:
            return "Int";
        case TypeTag::Bool:
            return "Bool";
        case TypeTag::Char:
            return "Char";
        case TypeTag::Float:
            return "Float";
        default:
            assert(false && "This should not happen");
    }
    return "Unknown";
}

TypeTag AtomType::GetTag() const {
    return tag_;
}

bool AtomType::IsSameAs(const AtomType& o) const {
    return tag_ == o.tag_;
}

std::string_view TupleType::GetName() const {
    return name_;
}

TypeTag TupleType::GetTag() const {
    return TypeTag::Tuple;
}

bool TupleType::IsSameAs(const TupleType& o) const {
    if(inner_types_.size() != o.inner_types_.size()) {
        return false;
    }
    for(size_t i = 0; i < inner_types_.size(); ++i) {
        if(!TypeIdentical(inner_types_[i], o.inner_types_[i])) {
            return false;
        }
    }
    return true;
}

const std::vector<const Type*>& TupleType::GetTupleTypes() const {
    return inner_types_;
}

TupleType::TupleType(std::vector<const Type*> inner_types)
    : inner_types_(std::move(inner_types)) {
    name_ = "(";

    for(size_t i = 0; i < inner_types_.size(); ++i) {
        name_ += GetTypeName(inner_types_[i]);
        if(i + 1 != inner_types_.size()) {
            name_ += ", ";
        }
    }

    name_ += ")";
}


std::string_view GetTypeName(const Type* type) {
    return std::visit([](const TypeLike auto& ut) -> std::string_view {
        return ut.GetName();
    }, *type);
}

TypeTag GetTypeTag(const Type* type) {
    return std::visit([](const TypeLike auto& ut) -> TypeTag {
        return ut.GetTag();
    }, *type);
}

bool TypeIdentical(const Type* t1, const Type* t2) {
    if(GetTypeTag(t1) != GetTypeTag(t2)) {
        return false;
    }

    return std::visit([t2]<TypeLike T>(const T& ut) -> bool {
        return std::get<T>(*t2).IsSameAs(ut);
    }, *t1);
}

}
