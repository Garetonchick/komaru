#include "type.hpp"

#include <format>
#include <cassert>

namespace komaru::lang {

std::deque<Type::Variant> Type::kStorage;
std::unordered_map<TypeTag, Type::Variant*> Type::kAtomTypesIndex;
std::unordered_map<TupleType::ID, Type::Variant*> Type::kTupleTypesIndex;
std::unordered_map<std::string, Type::Variant*> Type::kGenericTypesIndex;
std::unordered_map<std::string, Type::Variant*> Type::kFunctionTypesIndex;

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
            throw std::logic_error(
                std::format("Atomic type can't have tag {}", static_cast<int>(tag)));
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
    if (inner_types_.size() != o.inner_types_.size()) {
        return false;
    }
    for (size_t i = 0; i < inner_types_.size(); ++i) {
        if (inner_types_[i] != o.inner_types_[i]) {
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

    for (size_t i = 0; i < inner_types_.size(); ++i) {
        name_ += inner_types_[i].GetName();
        if (i + 1 != inner_types_.size()) {
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

    for (size_t i = 0; i < types.size(); ++i) {
        id += std::to_string(types[i].GetID());
        if (i + 1 != types.size()) {
            id += "_";
        }
    }

    return id;
}

GenericType::GenericType(std::string name)
    : name_(std::move(name)) {
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

FunctionType::FunctionType(const Type::Variant* source, const Type::Variant* target)
    : source_(source),
      target_(target) {
    name_ = std::format("{} -> {}", Source().GetName(), Target().GetName());
}

std::string_view FunctionType::GetName() const {
    return name_;
}

TypeTag FunctionType::GetTag() const {
    return TypeTag::Function;
}

Type FunctionType::Source() const {
    return Type(source_);
}

Type FunctionType::Target() const {
    return Type(target_);
}

bool FunctionType::operator==(const FunctionType& o) const {
    return Source() == o.Source() && Target() == o.Target();
}

Type::Type(const Variant* type)
    : type_(type) {
}

const Type::Variant* Type::GetVariantPointer() const {
    return type_;
}

// TODO: Support concurrency for Type constructors

Type Type::FromTag(TypeTag tag) {
    auto it = kAtomTypesIndex.find(tag);
    if (it != kAtomTypesIndex.end()) {
        return Type(it->second);
    }

    Variant* new_type = &kStorage.emplace_back(AtomType(tag));
    kAtomTypesIndex.emplace(tag, new_type);

    return Type(new_type);
}

Type Type::Tuple(std::vector<Type> types) {
    auto id = TupleType::GetIDFromTypes(types);
    auto it = kTupleTypesIndex.find(id);
    if (it != kTupleTypesIndex.end()) {
        return Type(it->second);
    }

    Variant* new_type = &kStorage.emplace_back(TupleType(std::move(types)));
    kTupleTypesIndex.emplace(std::move(id), new_type);

    return Type(new_type);
}

Type Type::TupleFromTags(std::vector<TypeTag> tags) {
    std::vector<Type> types;
    types.reserve(tags.size());

    for (TypeTag tag : tags) {
        types.emplace_back(FromTag(tag));
    }

    return Tuple(std::move(types));
}

Type Type::Function(Type source, Type target) {
    std::string key = std::to_string(source.GetID()) + "_" + std::to_string(target.GetID());
    auto it = kFunctionTypesIndex.find(key);
    if (it != kFunctionTypesIndex.end()) {
        return Type(it->second);
    }

    Variant* new_type = &kStorage.emplace_back(
        FunctionType(source.GetVariantPointer(), target.GetVariantPointer()));
    kFunctionTypesIndex.emplace(std::move(key), new_type);

    return Type(new_type);
}

Type Type::Generic(std::string name) {
    auto it = kGenericTypesIndex.find(name);

    if (it != kGenericTypesIndex.end()) {
        return Type(it->second);
    }

    Variant* new_type = &kStorage.emplace_back(GenericType(name));
    kGenericTypesIndex.emplace(std::move(name), new_type);

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

Type Type::Pow(size_t n) const {
    return Tuple(std::vector<Type>(n, *this));
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

Type operator*(Type t1, Type t2) {
    bool tup1 = t1.Holds<TupleType>();
    bool tup2 = t2.Holds<TupleType>();
    if (!tup1 && !tup2) {
        return Type::Tuple({t1, t2});
    }
    std::vector<Type> types;

    if (tup1) {
        types.insert_range(types.end(), t1.GetVariant<TupleType>().GetTupleTypes());
    } else {
        types.push_back(t1);
    }
    if (tup2) {
        types.insert_range(types.end(), t2.GetVariant<TupleType>().GetTupleTypes());
    } else {
        types.push_back(t2);
    }

    return Type::Tuple(types);
}

}  // namespace komaru::lang
