#pragma once

#include <komaru/util/non_copyable_non_movable.hpp>
#include <komaru/util/derive_variant.hpp>

#include <string>
#include <variant>
#include <vector>
#include <deque>
#include <span>
#include <unordered_map>

namespace komaru::lang {

class Type;

class CommonType;    // Int, Char, Either Int Char, Maybe Char, ...
class TupleType;     // (A, B, C)
class FunctionType;  // A -> B -> C
class ListType;      // [T]

template <typename T>
concept TypeLike = requires(const T t) {
    { t.GetName() } -> std::same_as<const std::string&>;
    { t.IsConcrete() } -> std::same_as<bool>;
} && std::equality_comparable<T>;

class Type : public util::DeriveVariant<Type> {
    using Variant = std::variant<CommonType, TupleType, FunctionType, ListType>;

public:
    explicit Type(const CommonType& type);
    explicit Type(const TupleType& type);
    explicit Type(const FunctionType& type);
    explicit Type(const ListType& type);

    Type(const Type& o) = default;
    Type& operator=(const Type& o) = default;

    static Type Simple(std::string name);
    static Type Parameterized(std::string name, std::vector<Type> params);
    static Type Tuple(std::vector<Type> types);
    static Type TupleFromSimples(std::vector<std::string> names);
    static Type Function(Type source, Type target);
    static Type FunctionChain(std::span<Type> types);
    static Type List(Type inner_type);
    static Type Var(std::string name);

    static Type Auto();
    static Type Singleton();
    static Type Int();
    static Type Float();
    static Type Double();
    static Type Char();
    static Type Bool();
    static Type String();

    const std::string& GetName() const;
    bool IsConcrete() const;
    std::uintptr_t GetID() const;
    Type Pow(size_t n) const;
    size_t GetComponentsNum() const;

    bool operator==(Type o) const;

    // For CRTP
    const Variant* GetVariantPointer() const;

private:
    explicit Type(const Variant* type);

    template <typename T>
    static Type::Variant* MakeType(const T& type);

    static std::deque<Variant>& GetStorage();
    static std::unordered_map<std::string, Variant*>& GetIndex();

private:
    const Variant* type_{nullptr};
};

class CommonType {
public:
    explicit CommonType(std::string main_name, std::vector<Type> params);

    const std::string& GetName() const;
    bool IsConcrete() const;
    std::string GetID() const;
    const std::string& GetMainName() const;
    const std::vector<Type>& GetParams() const;
    bool HasParams() const;

    bool operator==(const CommonType& o) const;

    static std::string MakeID(const std::string& name, const std::vector<Type>& params);

private:
    std::string main_name_;
    std::string name_;
    std::vector<Type> params_;
};

class TupleType {
public:
    explicit TupleType(std::vector<Type> inner_types);

    const std::string& GetName() const;
    bool IsConcrete() const;
    std::string GetID() const;
    const std::vector<Type>& GetTupleTypes() const;
    size_t GetTypesNum() const;

    bool operator==(const TupleType& o) const;

    static std::string MakeID(const std::vector<Type>& types);

private:
    std::string name_;
    std::vector<Type> inner_types_;
};

class FunctionType {
public:
    FunctionType(Type source, Type target);

    const std::string& GetName() const;
    bool IsConcrete() const;
    std::string GetID() const;
    Type Source() const;
    Type Target() const;

    bool operator==(const FunctionType& o) const;

    static std::string MakeID(Type source, Type target);

private:
    Type source_;
    Type target_;
    std::string name_;
};

class ListType {
public:
    explicit ListType(Type inner_type);

    const std::string& GetName() const;
    bool IsConcrete() const;
    std::string GetID() const;
    Type Inner() const;

    bool operator==(const ListType& o) const;

    static std::string MakeID(Type inner_type);

private:
    Type inner_type_;
    std::string name_;
};

static_assert(TypeLike<CommonType>);
static_assert(TypeLike<TupleType>);
static_assert(TypeLike<FunctionType>);
static_assert(TypeLike<ListType>);

static_assert(TypeLike<Type>);

Type operator*(Type t1, Type t2);
bool IsConcreteTypeName(const std::string& name);

}  // namespace komaru::lang

template <>
struct std::hash<komaru::lang::Type> {
    std::size_t operator()(const komaru::lang::Type& t) const noexcept {
        return t.GetID();
    }
};

template <>
struct std::hash<const komaru::lang::Type> {
    std::size_t operator()(const komaru::lang::Type& t) const noexcept {
        return t.GetID();
    }
};
