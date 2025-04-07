#include "type.hpp"

#include <format>
#include <cassert>

namespace komaru::lang {

std::deque<Type::Variant> Type::storage_;
std::unordered_map<TypeTag, Type::Variant*> Type::atom_types_index_;
std::unordered_map<TupleType::ID, Type::Variant*> Type::tuple_types_index_;
std::unordered_map<std::string, Type::Variant*> Type::generic_types_index_;

AtomType::AtomType(TypeTag tag)
    : tag_(tag) {
    switch (tag) {
        case TypeTag::Source:
        case TypeTag::Target:
        case TypeTag::Int:
        case TypeTag::Bool:
        case TypeTag::Char:
        case TypeTag::Float:
        case TypeTag::Singleton:
            return;
        default:
            throw std::logic_error(std::format(
                "Atomic type can't have tag {}", static_cast<int>(tag)
            ));
    }
}

std::string_view AtomType::GetName() const {
    switch (tag_) {
        case TypeTag::Singleton:
            return "S";
        case TypeTag::Source:
            return "Source";
        case TypeTag::Target:
            return "Sink";
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

bool AtomType::operator==(AtomType o) const {
    return tag_ == o.tag_;
}

std::string_view TupleType::GetName() const {
    return name_;
}

TypeTag TupleType::GetTag() const {
    return TypeTag::Tuple;
}

bool TupleType::operator==(const TupleType& o) const {
    if(inner_types_.size() != o.inner_types_.size()) {
        return false;
    }
    for(size_t i = 0; i < inner_types_.size(); ++i) {
        if(inner_types_[i] != o.inner_types_[i]) {
            return false;
        }
    }
    return true;
}

const std::vector<Type>& TupleType::GetTupleTypes() const {
    return inner_types_;
}

TupleType::TupleType(std::vector<Type> inner_types)
    : inner_types_(std::move(inner_types)) {
    name_ = "(";

    for(size_t i = 0; i < inner_types_.size(); ++i) {
        name_ += inner_types_[i].GetName();
        if(i + 1 != inner_types_.size()) {
            name_ += ", ";
        }
    }

    name_ += ")";
}

TupleType::ID TupleType::GetID() const {
    return GetIDFromTypes(inner_types_);
}

TupleType::ID TupleType::GetIDFromTypes(const std::vector<Type>& types) {
    TupleType::ID id;

    for(size_t i = 0; i < types.size(); ++i) {
        id += std::to_string(types[i].GetID());
        if(i + 1 != types.size()) {
            id += "_";
        }
    }

    return id;
}

GenericType::GenericType(std::string name) : name_(std::move(name)) {
}

std::string_view GenericType::GetName() const {
    return name_;
}

TypeTag GenericType::GetTag() const {
    return TypeTag::Generic;
}

const std::string& GenericType::GetID() const {
    return name_;
}

bool GenericType::operator==(const GenericType& o) const {
    return name_ == o.name_;
}

Type::Type(Variant* type) : type_(type) {
}

const Type::Variant* Type::GetVariantPointer() const {
    return type_;
}

// TODO: Support concurrency for Type constructors

Type Type::FromTag(TypeTag tag) {
    auto it = atom_types_index_.find(tag);
    if(it != atom_types_index_.end()) {
        return Type(it->second);
    }

    Variant* new_type = &storage_.emplace_back(AtomType(tag));
    atom_types_index_.emplace(tag, new_type);

    return Type(new_type);
}

Type Type::Tuple(std::vector<Type> types) {
    auto id = TupleType::GetIDFromTypes(types);
    auto it = tuple_types_index_.find(id);
    if(it != tuple_types_index_.end()) {
        return Type(it->second);
    }

    Variant* new_type = &storage_.emplace_back(TupleType(std::move(types)));
    tuple_types_index_.emplace(std::move(id), new_type);

    return Type(new_type);
}

Type Type::TupleFromTags(std::vector<TypeTag> tags) {
    std::vector<Type> types;
    types.reserve(tags.size());

    for(TypeTag tag : tags) {
        types.emplace_back(FromTag(tag));
    }

    return Tuple(std::move(types));
}

Type Type::Generic(std::string name) {
    auto it = generic_types_index_.find(name);

    if(it != generic_types_index_.end()) {
        return Type(it->second);
    }

    Variant* new_type = &storage_.emplace_back(GenericType(name));
    generic_types_index_.emplace(std::move(name), new_type);

    return Type(new_type);
}

Type Type::Auto() {
    return Generic("");
}

Type Type::Int() {
    return FromTag(TypeTag::Int);
}

Type Type::Float() {
    return FromTag(TypeTag::Float);
}

Type Type::Char() {
    return FromTag(TypeTag::Char);
}

Type Type::Bool() {
    return FromTag(TypeTag::Bool);
}

Type Type::Singleton() {
    return FromTag(TypeTag::Singleton);
}

Type Type::Source() {
    return FromTag(TypeTag::Source);
}

Type Type::Target() {
    return FromTag(TypeTag::Target);
}

std::string_view Type::GetName() const {
    return this->Visit([](const TypeLike auto& t) -> std::string_view {
        return t.GetName();
    });
}

TypeTag Type::GetTag() const {
    return this->Visit([](const TypeLike auto& t) -> TypeTag {
        return t.GetTag();
    });
}

std::uintptr_t Type::GetID() const {
    return reinterpret_cast<std::uintptr_t>(type_);
}

bool Type::operator==(Type o) const {
    // Because each type in storage is unique and never changes it's location
    // we can just compare pointers
    return type_ == o.type_;
}

}
