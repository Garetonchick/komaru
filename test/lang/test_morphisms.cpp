#include <gtest/gtest.h>

#include <komaru/lang/type.hpp>
#include <komaru/lang/morphism.hpp>
#include <komaru/util/std_extensions.hpp>

using namespace komaru::lang;
using namespace komaru::util;

TEST(Morphisms, Builtin) {
    auto ti = Type::FromTag(TypeTag::Int);
    auto ti2 = Type::TupleFromTags({TypeTag::Int, TypeTag::Int});
    auto plus = Morphism::Builtin(MorphismTag::Plus, ti2, ti);
    auto mul = Morphism::Builtin(MorphismTag::Multiply, ti2, ti);

    ASSERT_EQ(plus->GetName(), "+");
    ASSERT_EQ(plus->GetSource(), ti2);
    ASSERT_EQ(plus->GetTarget(), ti);
    ASSERT_EQ(plus->GetTag(), MorphismTag::Plus);
    ASSERT_EQ(mul->GetTag(), MorphismTag::Multiply);
}

TEST(Morphisms, Compound) {
    auto ti = Type::FromTag(TypeTag::Int);
    auto ti2 = Type::TupleFromTags({TypeTag::Int, TypeTag::Int});
    auto start_morphism = Morphism::Builtin(MorphismTag::Id, Type::FromTag(TypeTag::Source),
                                            Type::FromTag(TypeTag::Singleton));
    auto value_morphism = Morphism::WithValue("some_value", Value::TupleFromAtoms(9, 42));
    auto plus_morphism = Morphism::Builtin(MorphismTag::Plus, ti2, ti);
    auto end_morphism = Morphism::Builtin(MorphismTag::Id, Type::FromTag(TypeTag::Int),
                                          Type::FromTag(TypeTag::Target));
    std::vector<MorphismPtr> program_morphisms = {start_morphism, value_morphism, plus_morphism,
                                                  end_morphism};
    auto program_morphism = Morphism::Compound("main", program_morphisms);

    ASSERT_EQ(program_morphism->GetName(), "main");
    ASSERT_EQ(program_morphism->GetSource(), Type::FromTag(TypeTag::Source));
    ASSERT_EQ(program_morphism->GetTarget(), Type::FromTag(TypeTag::Target));
    ASSERT_EQ(program_morphism->GetTag(), MorphismTag::Compound);

    program_morphism->Visit(Overloaded{[&](const CompoundMorphism& morphism) {
                                           const auto& submorphisms = morphism.GetMorphisms();
                                           ASSERT_EQ(submorphisms.size(), program_morphisms.size());
                                           for (size_t i = 0; i < program_morphisms.size(); ++i) {
                                               ASSERT_EQ(submorphisms[i]->GetName(),
                                                         program_morphisms[i]->GetName());
                                           }
                                       },
                                       [](const auto&) {
                                           FAIL();
                                       }});
}
