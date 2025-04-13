#pragma once

#include <util/non_copyable_non_movable.hpp>
#include <util/derive_variant.hpp>

#include <string>
#include <variant>
#include <vector>
#include <deque>
#include <unordered_map>

namespace komaru::lang {

// TODO: Type tags should only be used for the builtin types
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

class AtomType;   // Int, Char, Bool, Float, ...
class TupleType;  // (A, B, C)
// class ListType; // [T]
// class GenericType; // T
// class StructType; // struct MyStruct {i: Int, b: Bool}

template <typename T>
concept TypeLike = requires(const T t) {
    { t.GetName() } -> std::convertible_to<std::string_view>;
    { t.GetTag() } -> std::same_as<TypeTag>;
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
    using ID = std::string;  // TODO: change to something like vector of tags

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

class GenericType {
public:
    explicit GenericType(std::string name);

    std::string_view GetName() const;
    TypeTag GetTag() const;
    const std::string& GetID() const;

    bool operator==(const GenericType& o) const;

private:
    std::string name_;
};

static_assert(TypeLike<AtomType>);
static_assert(TypeLike<TupleType>);
static_assert(TypeLike<GenericType>);

class Type : public util::DeriveVariant<Type> {
    using Variant = std::variant<AtomType, TupleType, GenericType>;

public:
    static Type FromTag(TypeTag tag);
    static Type Tuple(std::vector<Type> types);
    static Type TupleFromTags(std::vector<TypeTag> tags);

    static Type Generic(std::string name);
    static Type Auto();

    static Type Int();
    static Type Float();
    static Type Char();
    static Type Bool();
    static Type Singleton();
    static Type Source();
    static Type Target();

    std::string_view GetName() const;
    TypeTag GetTag() const;
    std::uintptr_t GetID() const;
    Type Pow(size_t n) const;

    bool operator==(Type o) const;

    // For CRTP
    const Variant* GetVariantPointer() const;

private:
    explicit Type(Variant* type);

private:
    const Variant* type_{nullptr};

    // Deque guarantees no pointer invalidation happens
    static std::deque<Variant> kStorage;
    static std::unordered_map<TypeTag, Variant*> kAtomTypesIndex;
    static std::unordered_map<TupleType::ID, Variant*> kTupleTypesIndex;
    static std::unordered_map<std::string, Variant*> kGenericTypesIndex;
};

Type operator*(Type t1, Type t2);

static_assert(TypeLike<Type>);

}  // namespace komaru::lang
