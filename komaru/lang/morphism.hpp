#pragma once
#include <komaru/lang/type.hpp>
#include <komaru/lang/literal.hpp>
#include <komaru/util/derive_variant.hpp>

#include <map>

namespace komaru::lang {

class Morphism;
using MorphismPtr = std::shared_ptr<Morphism>;

class CommonMorphism;
class PositionMorphism;
class BindedMorphism;
class LiteralMorphism;
// class TupleMorphism;
// class ListMorphism;

template <typename T>
concept MorphismLike = requires(const T t) {
    { t.ToString() } -> std::same_as<std::string>;
    { t.GetSource() } -> std::same_as<Type>;
    { t.GetTarget() } -> std::same_as<Type>;
    { t.GetType() } -> std::same_as<Type>;
    { t.GetParamNum() } -> std::same_as<size_t>;
    { t.IsValue() } -> std::same_as<bool>;
    { t.ShouldBeShielded() } -> std::same_as<bool>;
    { t.IsOperator() } -> std::same_as<bool>;
};

class CommonMorphism {
public:
    CommonMorphism(std::string name, Type source, Type target);

    std::string ToString() const;
    Type GetSource() const;
    Type GetTarget() const;
    Type GetType() const;
    size_t GetParamNum() const;
    bool IsValue() const;
    bool ShouldBeShielded() const;
    bool IsOperator() const;
    const std::string& GetName() const;

private:
    std::string name_;
    Type source_;
    Type target_;
};

class PositionMorphism {
public:
    explicit PositionMorphism(size_t pos);

    std::string ToString() const;
    Type GetSource() const;
    Type GetTarget() const;
    Type GetType() const;
    size_t GetParamNum() const;
    bool IsValue() const;
    bool ShouldBeShielded() const;
    bool IsOperator() const;
    size_t GetPosition() const;
    bool IsNonePosition() const;

private:
    size_t pos_;
};

class BindedMorphism {
public:
    BindedMorphism(MorphismPtr morphism, std::map<size_t, MorphismPtr> mapping);

    std::string ToString() const;
    Type GetSource() const;
    Type GetTarget() const;
    Type GetType() const;
    size_t GetParamNum() const;
    bool IsValue() const;
    bool ShouldBeShielded() const;
    bool IsOperator() const;
    const MorphismPtr& GetUnderlyingMorphism() const;
    const std::map<size_t, MorphismPtr>& GetMapping() const;

private:
    std::string ToStringAsOperator() const;

private:
    MorphismPtr morphism_;
    std::map<size_t, MorphismPtr> mapping_;
    Type source_;
    Type target_;
};

// Always: S -> *Some Type*
class LiteralMorphism {
public:
    explicit LiteralMorphism(Literal literal);

    std::string ToString() const;
    Type GetSource() const;
    Type GetTarget() const;
    Type GetType() const;
    const Literal& GetLiteral() const;
    size_t GetParamNum() const;
    bool IsValue() const;
    bool ShouldBeShielded() const;
    bool IsOperator() const;

private:
    Literal literal_;
};

class Morphism : public util::DeriveVariant<Morphism> {
    using Variant = std::variant<CommonMorphism, PositionMorphism, BindedMorphism, LiteralMorphism>;

    struct PrivateDummy {};

public:
    template <typename T>
    Morphism(PrivateDummy, T morphism)
        : morphism_(std::move(morphism)) {
    }

    static MorphismPtr Common(std::string name, Type source, Type target);
    static MorphismPtr CommonWithType(std::string name, Type type);
    static MorphismPtr Value(std::string name, Type type);
    static MorphismPtr ChainFunction(std::string name, const std::vector<Type>& types);
    static MorphismPtr Position(size_t pos);
    static MorphismPtr NonePosition();
    static MorphismPtr Binded(MorphismPtr morphism, std::map<size_t, MorphismPtr> mapping);
    static MorphismPtr Literal(Literal literal);

    static MorphismPtr Plus();
    static MorphismPtr Minus();
    static MorphismPtr Multiply();
    static MorphismPtr Greater();
    static MorphismPtr Less();
    static MorphismPtr GreaterEq();
    static MorphismPtr LessEq();
    static MorphismPtr Identity();
    static MorphismPtr True();
    static MorphismPtr False();

    std::string ToString() const;
    Type GetSource() const;
    Type GetTarget() const;
    Type GetType() const;
    size_t GetParamNum() const;
    bool IsValue() const;
    bool ShouldBeShielded() const;
    bool IsOperator() const;

    // For CRTP
    const Variant* GetVariantPointer() const;

private:
    static bool ValidateCompound(const std::vector<MorphismPtr>& morphisms);

private:
    Variant morphism_;
};

static_assert(MorphismLike<CommonMorphism>);
static_assert(MorphismLike<PositionMorphism>);
static_assert(MorphismLike<BindedMorphism>);
static_assert(MorphismLike<LiteralMorphism>);

static_assert(MorphismLike<Morphism>);

bool IsOperatorName(const std::string& name);
bool IsFunctionName(const std::string& name);
bool IsConstructorName(const std::string& name);
std::string ToStringShielded(const Morphism& morphism);

}  // namespace komaru::lang
