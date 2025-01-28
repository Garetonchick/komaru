#pragma once

#include <util/non_copyable_non_movable.hpp>

#include <string>
#include <variant>
#include <vector>

namespace komaru::lang {

enum class TypeTag {
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

using Type = std::variant<AtomType, TupleType>;

template<typename T>
concept TypeLike = requires(const T t, const T o) {
    {t.GetName()} -> std::convertible_to<std::string_view>;
    {t.GetTag()} -> std::same_as<TypeTag>;
    {t.IsSameAs(o)} -> std::same_as<bool>;
} && !std::equality_comparable<T>;

class TypeRegistry;

class AtomType {
    friend TypeRegistry;
public:
    std::string_view GetName() const;
    TypeTag GetTag() const;
    bool IsSameAs(const AtomType& o) const;

private:
    explicit AtomType(TypeTag tag);

private:
    TypeTag tag_;
};

class TupleType {
    friend TypeRegistry;
public:
    std::string_view GetName() const;
    TypeTag GetTag() const;
    bool IsSameAs(const TupleType& o) const;
    const std::vector<const Type*>& GetTupleTypes() const;

private:
    explicit TupleType(std::vector<const Type*> inner_types);

private:
    std::string name_;
    std::vector<const Type*> inner_types_;
};

static_assert(TypeLike<AtomType>);
static_assert(TypeLike<TupleType>);

std::string_view GetTypeName(const Type* type);
TypeTag GetTypeTag(const Type* type);
bool TypeIdentical(const Type* t1, const Type* t2);

// class TypeRegistry;

// class Type : util::NonCopyable {
//     friend TypeRegistry;
// public:
//     const std::string& GetName() const;
//     TypeTag GetBaseType() const;
//     const std::vector<const Type*>& GetInnerTypes() const;

//     bool operator==(const Type& o) const = delete; // Use IsSameAs
//     bool IsSameAs(const Type& o) const;

// private:
//     Type(std::string name, TypeTag type);
//     Type(std::string name, TypeTag base_type, std::vector<const Type*> inner_types);

// private:
//     std::string name_;
//     TypeTag base_type_;
//     std::vector<const Type*> inner_types_;
// };

}
