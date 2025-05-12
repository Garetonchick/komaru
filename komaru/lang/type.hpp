#pragma once

#include <komaru/util/non_copyable_non_movable.hpp>
#include <komaru/util/derive_variant.hpp>
#include <komaru/lang/style.hpp>

#include <string>
#include <variant>
#include <vector>
#include <deque>
#include <map>
#include <unordered_map>

namespace komaru::lang {

class Type;

class CommonType;    // Int, Char, Either Int Char, Maybe Char, ...
class TupleType;     // (A, B, C)
class FunctionType;  // A -> B -> C
class ListType;      // [T]

template <typename T>
concept TypeLike = requires(const T t) {
    { t.ToString() } -> std::same_as<std::string>;
    { t.ToString(Style::Komaru) } -> std::same_as<std::string>;
    { t.IsConcrete() } -> std::same_as<bool>;
    { t.ShouldBeShielded() } -> std::same_as<bool>;
} && std::equality_comparable<T>;

class Type : public util::DeriveVariant<Type> {
    using Variant = std::variant<CommonType, TupleType, FunctionType, ListType>;

public:
    Type()
        : type_(Type::Auto().GetVariantPointer()) {
    }

    explicit Type(const CommonType& type);
    explicit Type(const TupleType& type);
    explicit Type(const FunctionType& type);
    explicit Type(const ListType& type);

    Type(const Type& o) = default;
    Type& operator=(const Type& o) = default;

    static Type Common(std::string name);
    static Type Simple(std::string name);
    static Type Parameterized(std::string name, std::vector<Type> params);
    static Type Tuple(std::vector<Type> types);
    static Type TupleFromSimples(std::vector<std::string> names);
    static Type Function(Type source, Type target);
    static Type FunctionChain(const std::vector<Type>& types);
    static Type Var(std::string name);
    static Type List(Type inner_type);

    static Type Auto();
    static Type Singleton();
    static Type Int();
    static Type Float();
    static Type Double();
    static Type Char();
    static Type Bool();
    static Type String();

    std::string ToString(Style style = Style::Komaru) const;
    bool IsConcrete() const;
    bool ShouldBeShielded() const;
    std::uintptr_t GetID() const;
    Type Pow(size_t n) const;
    size_t GetComponentsNum() const;
    std::vector<Type> GetComponents() const;
    size_t GetParamNum() const;
    bool IsValueType() const;
    std::vector<Type> FlattenFunction() const;
    bool IsTypeVar() const;
    size_t TypeVariantIndex() const;
    Type Pure() const;

    bool operator==(Type o) const;
    bool operator<(Type o) const;  // Used for containers like std::map

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

class TypeConstructor {
public:
    TypeConstructor(std::string name, size_t num_params);

    std::string ToString(Style style = Style::Komaru) const;
    size_t GetNumParams() const;

    bool operator==(const TypeConstructor& o) const;
    bool operator<(const TypeConstructor& o) const;

private:
    std::string name_;
    size_t num_params_;
};

class CommonType {
public:
    explicit CommonType(std::string name, std::vector<Type> params);

    std::string ToString(Style style = Style::Komaru) const;
    bool IsConcrete() const;
    bool ShouldBeShielded() const;
    std::string GetID() const;
    const std::string& GetName() const;
    const std::vector<Type>& GetTypeParams() const;
    size_t NumTypeParams() const;
    bool HasTypeParams() const;
    bool IsAuto() const;

    bool operator==(const CommonType& o) const;

    static std::string MakeID(const std::string& name, const std::vector<Type>& params);

private:
    std::string ToStringDebug() const;

private:
    std::string name_;
    std::vector<Type> params_;
};

class TupleType {
public:
    explicit TupleType(std::vector<Type> inner_types);

    std::string ToString(Style style = Style::Komaru) const;
    bool IsConcrete() const;
    bool ShouldBeShielded() const;
    std::string GetID() const;
    const std::vector<Type>& GetTupleTypes() const;
    size_t GetTypesNum() const;

    bool operator==(const TupleType& o) const;

    static std::string MakeID(const std::vector<Type>& types);

private:
    std::string ToStringKomaru() const;
    std::string ToStringHaskell() const;
    std::string ToStringDebug() const;

private:
    std::vector<Type> inner_types_;
};

class FunctionType {
public:
    FunctionType(Type source, Type target);

    std::string ToString(Style style = Style::Komaru) const;
    bool IsConcrete() const;
    bool ShouldBeShielded() const;
    std::string GetID() const;
    Type Source() const;
    Type Target() const;
    size_t GetParamNum() const;
    bool IsValueType() const;

    bool operator==(const FunctionType& o) const;

    static std::string MakeID(Type source, Type target);

private:
    Type source_;
    Type target_;
};

class ListType {
public:
    explicit ListType(Type inner_type);

    std::string ToString(Style style = Style::Komaru) const;
    bool IsConcrete() const;
    bool ShouldBeShielded() const;
    std::string GetID() const;
    Type Inner() const;

    bool operator==(const ListType& o) const;

    static std::string MakeID(Type inner_type);

private:
    Type inner_type_;
};

static_assert(TypeLike<CommonType>);
static_assert(TypeLike<TupleType>);
static_assert(TypeLike<FunctionType>);
static_assert(TypeLike<ListType>);

static_assert(TypeLike<Type>);

using MatchMap = std::map<std::string, std::variant<Type, TypeConstructor>>;

Type operator*(Type t1, Type t2);
bool IsConcreteTypeName(const std::string& name);
std::optional<Type> TryDeduceTypes(Type func_type, Type arg_type);
std::optional<Type> TryDeduceTypes(Type func_type, const std::map<size_t, Type>& arg_mapping);
Type DeduceTypes(Type func_type, Type arg_type);
Type DeduceTypes(Type func_type, const std::map<size_t, Type>& arg_mapping);
// It will automatically deduce types too
std::optional<Type> TryMakeSubstitution(Type func_type, const std::map<size_t, Type>& arg_mapping);
Type MakeSubstitution(Type func_type, const std::map<size_t, Type>& arg_mapping);
[[nodiscard]] bool MergeMatchMaps(MatchMap& mapping, const MatchMap& sub_mapping);
std::optional<MatchMap> TryMatchTypes(Type param_type, Type arg_type);
MatchMap MatchTypes(Type param_type, Type arg_type);
Type ApplyMatchMap(Type type, const MatchMap& mapping);
bool CanBeSubstituted(Type param_type, Type arg_type, const MatchMap& mapping = {});
Type CurryFunction(Type source, Type target);
std::string ArgMappingToString(const std::map<size_t, Type>& arg_mapping);

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
