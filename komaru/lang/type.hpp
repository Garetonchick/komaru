#pragma once

#include <util/non_copyable_non_movable.hpp>
#include <util/derive_variant.hpp>

#include <string>
#include <variant>
#include <vector>
#include <deque>
#include <unordered_map>

namespace komaru::lang {

enum class TypeTag {
    Singleton,
    Source,
    Target,
    Int,
    Char,
    Bool,
    Float,
    Tuple,
    List,
    Generic,
};

class AtomType; // Int, Char, Bool, Float, ...
class TupleType; // (A, B, C)
// class ListType; // [T]
// class GenericType; // T
// class StructType; // struct MyStruct {i: Int, b: Bool}

template<typename T>
concept TypeLike = requires(const T t) {
    {t.GetName()} -> std::convertible_to<std::string_view>;
    {t.GetTag()} -> std::same_as<TypeTag>;
} && std::equality_comparable<T>;

class Type;

class AtomType {
public:
    explicit AtomType(TypeTag tag);

    std::string_view GetName() const;
    TypeTag GetTag() const;

    bool operator==(AtomType o) const;

private:
    TypeTag tag_;
};

class TupleType {
public:
    using ID = std::string; // TODO: change to something like vector of tags

public:
    explicit TupleType(std::vector<Type> inner_types);

    std::string_view GetName() const;
    TypeTag GetTag() const;
    const std::vector<Type>& GetTupleTypes() const;
    ID GetID() const;

    bool operator==(const TupleType& o) const;

    static ID GetIDFromTypes(const std::vector<Type>& types);

private:
    std::string name_;
    std::vector<Type> inner_types_;
};

static_assert(TypeLike<AtomType>);
static_assert(TypeLike<TupleType>);

class Type : public util::DeriveVariant<Type> {
    using Variant = std::variant<AtomType, TupleType>;
public:
    static Type FromTag(TypeTag tag);
    static Type Tuple(std::vector<Type> types);
    static Type TupleFromTags(std::vector<TypeTag> tags);

    std::string_view GetName() const;
    TypeTag GetTag() const;
    std::uintptr_t GetID() const;

    bool operator==(Type o) const;

    // For CRTP
    const Variant* GetVariantPointer() const;

private:
    explicit Type(Variant* type);

private:
    const Variant* type_{nullptr};

    // Deque guarantees no pointer invalidation happens
    static std::deque<Variant> storage_;
    static std::unordered_map<TypeTag, Variant*> atom_types_index_;
    static std::unordered_map<TupleType::ID, Variant*> tuple_types_index_;
};

static_assert(TypeLike<Type>);

}
