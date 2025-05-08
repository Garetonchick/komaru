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
