#include <gtest/gtest.h>

#include <komaru/lang/type.hpp>
#include <komaru/lang/morphism.hpp>
#include <komaru/util/std_extensions.hpp>

using namespace komaru::lang;
using namespace komaru::util;

TEST(Morphisms, Builtin) {
    auto plus = Morphism::Plus();
    auto mul = Morphism::Multiply();

    ASSERT_EQ(plus->ToString(), "+");
    ASSERT_EQ(mul->ToString(), "*");
    ASSERT_TRUE(plus->GetSource().IsTypeVar());
    ASSERT_TRUE(!plus->GetTarget().IsConcrete());
    ASSERT_TRUE(plus->IsOperator());
    ASSERT_TRUE(mul->IsOperator());
    ASSERT_EQ(plus->GetParamNum(), 2);
    ASSERT_EQ(mul->GetParamNum(), 2);
    ASSERT_EQ(plus->GetType().FlattenFunction().size(), 3);
    ASSERT_EQ(mul->GetType().FlattenFunction().size(), 3);
}

TEST(Morphisms, Binded) {
    auto plus = Morphism::Plus();

    auto binded = Morphism::Binded(plus, {{0, Morphism::CommonWithType("a", Type::Int())},
                                          {1, Morphism::CommonWithType("b", Type::Int())}});

    ASSERT_EQ(binded->ToString(), "a + b");
    ASSERT_EQ(binded->GetType().ToString(), "S -> Int");
}

TEST(Morphisms, RBind) {
    auto plus = Morphism::Plus();
    auto binded = Morphism::Binded(plus, {{1, Morphism::Literal(Literal::Number(42))}});

    ASSERT_EQ(binded->ToString(), "+ 42");
    ASSERT_EQ(binded->GetType().ToString(), "a -> a");
}
