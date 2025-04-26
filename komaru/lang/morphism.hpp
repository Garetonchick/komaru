#pragma once
#include <komaru/lang/type.hpp>
#include <komaru/lang/value.hpp>
#include <komaru/util/derive_variant.hpp>

#include <map>

namespace komaru::lang {

// TODO: Morphism tags should only be used for the builtin morphisms
enum class MorphismTag {
    Compound,
    Value,
    Id,
    Plus,
    Minus,
    Multiply,
    Less,
    LessEq,
    Greater,
    GreaterEq,
    DebugInt,
    Position,
    Binded,
    Name,
};

class Morphism;

// TODO: What about cyclic references?
using MorphismPtr = std::shared_ptr<Morphism>;

class BuiltinMorphism {
public:
    BuiltinMorphism(MorphismTag tag, Type source, Type target);

    const std::string& GetName() const;
    Type GetSource() const;
    Type GetTarget() const;
    MorphismTag GetTag() const;
    std::string ToString() const;

private:
    std::string name_;
    MorphismTag tag_;
    Type source_type_;
    Type target_type_;
};

// Composition of several morphisms
class CompoundMorphism {
public:
    CompoundMorphism(std::string name, std::vector<MorphismPtr> morphisms);

    const std::string& GetName() const;
    Type GetSource() const;
    Type GetTarget() const;
    MorphismTag GetTag() const;
    std::string ToString() const;

    const std::vector<MorphismPtr>& GetMorphisms() const;

private:
    std::string name_;
    std::vector<MorphismPtr> morphisms_;
};

// Singleton -> *Some Type*
class ValueMorphism {
public:
    ValueMorphism(std::string name, Value value, bool strict = true);

    const std::string& GetName() const;
    Type GetSource() const;
    Type GetTarget() const;
    MorphismTag GetTag() const;
    const Value& GetValue() const;
    MorphismPtr Unrestricted() const;
    std::string ToString() const;

private:
    std::string name_;
    Value value_;
    bool strict_;
};

class PositionMorphism {
public:
    explicit PositionMorphism(size_t pos);

    const std::string& GetName() const;
    Type GetSource() const;
    Type GetTarget() const;
    MorphismTag GetTag() const;
    std::string ToString() const;

    size_t GetPosition() const;
    bool IsNonePosition() const;

private:
    size_t pos_;
    std::string name_;
};

class BindedMorphism {
public:
    BindedMorphism(MorphismPtr morphism, std::map<size_t, MorphismPtr> mapping);

    const std::string& GetName() const;
    Type GetSource() const;
    Type GetTarget() const;
    MorphismTag GetTag() const;
    std::string ToString() const;

    const MorphismPtr& GetUnderlyingMorphism() const;
    const std::map<size_t, MorphismPtr>& GetMapping() const;

private:
    MorphismPtr morphism_;
    std::map<size_t, MorphismPtr> mapping_;
};

class NameMorphism {
public:
    NameMorphism(std::string name, Type source, Type target);

    const std::string& GetName() const;
    Type GetSource() const;
    Type GetTarget() const;
    MorphismTag GetTag() const;
    std::string ToString() const;

private:
    std::string name_;
    Type source_;
    Type target_;
};

class Morphism : public util::DeriveVariant<Morphism> {
    using Variant = std::variant<BuiltinMorphism, CompoundMorphism, ValueMorphism, PositionMorphism,
                                 BindedMorphism, NameMorphism>;

    friend MorphismPtr BindMorphism(MorphismPtr morphism, std::map<size_t, MorphismPtr> mapping);

    struct PrivateDummy {};

public:
    template <typename T>
    Morphism(PrivateDummy, T morphism)
        : morphism_(std::move(morphism)) {
    }

    static MorphismPtr Builtin(MorphismTag tag, Type source, Type target);
    static MorphismPtr Compound(std::string name, std::vector<MorphismPtr> morphisms);
    static MorphismPtr WithValue(std::string name, Value value, bool strict = true);
    static MorphismPtr Position(size_t pos);
    static MorphismPtr NonePosition();
    static MorphismPtr WithName(std::string name, Type source, Type target);

    // TODO: const std::string& vs std::string_view vs std::string vs ???
    const std::string& GetName() const;
    Type GetSource() const;
    Type GetTarget() const;
    MorphismTag GetTag() const;
    std::string ToString() const;
    // TODO: GetType()

    // For CRTP
    const Variant* GetVariantPointer() const;

private:
    static bool ValidateCompound(const std::vector<MorphismPtr>& morphisms);

private:
    Variant morphism_;
};

MorphismPtr BindMorphism(MorphismPtr morphism, std::map<size_t, MorphismPtr> mapping);
bool IsOperator(const Morphism& morphism);
bool IsComplex(const Morphism& morphism);
bool IsFunction(const Morphism& morphism);

}  // namespace komaru::lang
