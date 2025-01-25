#include <gtest/gtest.h>
#include <lang/type_registry.hpp>

using namespace komaru::lang;

// Demonstrate some basic assertions.
TEST(TypeRegistry, Basic) {
    TypeRegistry type_registry;

    auto& ti = type_registry.RegisterType("Int", BuiltinType::Int);
    auto& tc = type_registry.RegisterType("Char", BuiltinType::Char);
    auto& tf = type_registry.RegisterType("Float", BuiltinType::Float);

    ASSERT_EQ(type_registry.GetNumTypes(), 3);

    ASSERT_TRUE(ti.IsSameAs(type_registry.GetType(0)));
    ASSERT_TRUE(tc.IsSameAs(type_registry.GetType(1)));
    ASSERT_TRUE(tf.IsSameAs(type_registry.GetType(2)));

    ASSERT_EQ(ti.GetName(), "Int");
    ASSERT_EQ(tc.GetName(), "Char");
    ASSERT_EQ(tf.GetName(), "Float");

    ASSERT_EQ(ti.GetBaseType(), BuiltinType::Int);
    ASSERT_EQ(tc.GetBaseType(), BuiltinType::Char);
    ASSERT_EQ(tf.GetBaseType(), BuiltinType::Float);

    ASSERT_TRUE(ti.GetInnerTypes().empty());
    ASSERT_TRUE(tc.GetInnerTypes().empty());
    ASSERT_TRUE(tf.GetInnerTypes().empty());
}

