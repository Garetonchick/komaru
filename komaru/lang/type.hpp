#pragma once

#include <komaru/util/non_copyable_non_movable.hpp>
#include <komaru/util/derive_variant.hpp>

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
    Function,
    Parameterized,
};

class AtomType;   // Int, Char, Bool, Float, ...
class TupleType;  // (A, B, C)
class FunctionType;
// class ListType; // [T]
// class RecordType; // struct MyStruct {i: Int, b: Bool}

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
    size_t GetTypesNum() const;
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

class ParameterizedType {
public:
    ParameterizedType(const std::string& name, std::vector<Type> params);

    std::string_view GetMainName() const;
    std::string_view GetName() const;
    TypeTag GetTag() const;
    std::string GetID() const;
    const std::vector<Type>& GetParamTypes() const;

    bool operator==(const ParameterizedType& o) const;

    static std::string CalcID(const std::string& name, const std::vector<Type>& params);

private:
    std::string main_name_;
    std::string name_;
    std::vector<Type> params_;
};

static_assert(TypeLike<AtomType>);
static_assert(TypeLike<TupleType>);
static_assert(TypeLike<GenericType>);
static_assert(TypeLike<ParameterizedType>);

class Type : public util::DeriveVariant<Type> {
    using Variant = std::variant<AtomType, TupleType, GenericType, FunctionType, ParameterizedType>;
    friend class FunctionType;

public:
    Type(const Type& o) = default;
    Type& operator=(const Type& o) = default;

    static Type FromTag(TypeTag tag);
    static Type Tuple(std::vector<Type> types);
    static Type TupleFromTags(std::vector<TypeTag> tags);
    static Type Function(Type source, Type target);
    static Type Parameterized(std::string name, std::vector<Type> params);

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
    size_t NumComponents() const;

    bool operator==(Type o) const;

    // For CRTP
    const Variant* GetVariantPointer() const;

private:
    explicit Type(const Variant* type);

private:
    const Variant* type_{nullptr};

    // Deque guarantees no pointer invalidation happens
    static std::deque<Variant> kStorage;
    static std::unordered_map<TypeTag, Variant*> kAtomTypesIndex;
    static std::unordered_map<TupleType::ID, Variant*> kTupleTypesIndex;
    static std::unordered_map<std::string, Variant*> kGenericTypesIndex;
    static std::unordered_map<std::string, Variant*> kFunctionTypesIndex;
    static std::unordered_map<std::string, Variant*> kParameterizedTypesIndex;
};

class FunctionType {
public:
    FunctionType(const Type::Variant* source, const Type::Variant* target);

    std::string_view GetName() const;
    TypeTag GetTag() const;
    Type Source() const;
    Type Target() const;

    bool operator==(const FunctionType& o) const;

private:
    const Type::Variant* source_;
    const Type::Variant* target_;
    std::string name_;
};

Type operator*(Type t1, Type t2);

static_assert(TypeLike<FunctionType>);
static_assert(TypeLike<Type>);

}  // namespace komaru::lang
