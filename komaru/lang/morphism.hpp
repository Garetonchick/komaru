#pragma once
#include <lang/type.hpp>
#include <lang/value.hpp>
#include <util/derive_variant.hpp>

namespace komaru::lang {

enum class MorphismTag {
    Compound,
    Value,
    Id,
    Plus,
    Minus,
    Multiply,
    DebugInt,
};

class Morphism;

// TODO: What about cyclic references?
using MorphismPtr = std::shared_ptr<Morphism>;

class BuiltinMorphism {
public:
    BuiltinMorphism(MorphismTag tag, Type source, Type target);

    std::string_view GetName() const;
    Type GetSource() const;
    Type GetTarget() const;
    MorphismTag GetTag() const;

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

    std::string_view GetName() const;
    Type GetSource() const;
    Type GetTarget() const;
    MorphismTag GetTag() const;

    const std::vector<MorphismPtr>& GetMorphisms() const;

private:
    std::string name_;
    std::vector<MorphismPtr> morphisms_;
};

// Singleton -> *Some Type*
class ValueMorphism {
public:
    ValueMorphism(std::string name, Value value);

    std::string_view GetName() const;
    Type GetSource() const;
    Type GetTarget() const;
    MorphismTag GetTag() const;

private:
    std::string name_;
    Value value_;
};

class Morphism : public util::DeriveVariant<Morphism> {
    using Variant = std::variant<BuiltinMorphism, CompoundMorphism, ValueMorphism>;

    struct PrivateDummy {};
public:
    template<typename T>
    Morphism(PrivateDummy, T morphism) : morphism_(std::move(morphism)) {}

    static MorphismPtr Builtin(MorphismTag tag, Type source, Type target);
    static MorphismPtr Compound(std::string name, std::vector<MorphismPtr> morphisms);
    static MorphismPtr WithValue(std::string name, Value value);

    std::string_view GetName() const;
    Type GetSource() const;
    Type GetTarget() const;
    MorphismTag GetTag() const;
    // TODO: GetType()

    // For CRTP
    const Variant* GetVariantPointer() const;

private:
    static bool ValidateCompound(const std::vector<MorphismPtr>& morphisms);

private:
    Variant morphism_;
};

}
