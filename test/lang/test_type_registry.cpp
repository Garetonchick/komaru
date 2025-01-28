#include <gtest/gtest.h>
#include <lang/type_registry.hpp>

using namespace komaru::lang;

TEST(TypeRegistry, Basic) {
    TypeRegistry type_registry;

    const Type* ti = type_registry.RegisterAtomType(TypeTag::Int);
    const Type* tc = type_registry.RegisterAtomType(TypeTag::Char);
    const Type* tf = type_registry.RegisterAtomType(TypeTag::Float);
    const Type* tup = type_registry.RegisterTupleType({ti, tc, tf});

    ASSERT_EQ(type_registry.GetNumTypes(), 4);

    ASSERT_TRUE(TypeIdentical(ti, type_registry.GetType(0)));
    ASSERT_TRUE(TypeIdentical(tc, type_registry.GetType(1)));
    ASSERT_TRUE(TypeIdentical(tf, type_registry.GetType(2)));

    ASSERT_EQ(GetTypeName(ti), "Int");
    ASSERT_EQ(GetTypeName(tc), "Char");
    ASSERT_EQ(GetTypeName(tf), "Float");

    ASSERT_EQ(GetTypeTag(ti), TypeTag::Int);
    ASSERT_EQ(GetTypeTag(tc), TypeTag::Char);
    ASSERT_EQ(GetTypeTag(tf), TypeTag::Float);

    ASSERT_TRUE(std::holds_alternative<AtomType>(*ti));
    ASSERT_TRUE(std::holds_alternative<AtomType>(*tc));
    ASSERT_TRUE(std::holds_alternative<AtomType>(*tf));

    ASSERT_TRUE(std::holds_alternative<TupleType>(*tup));
    const TupleType& as_tup = std::get<TupleType>(*tup);
    const auto& tup_types = as_tup.GetTupleTypes();
    std::vector<const Type*> expected_tup_types = {ti, tc, tf};

    ASSERT_EQ(tup_types.size(), 3);

    for(size_t i = 0; i < tup_types.size(); ++i) {
        ASSERT_TRUE(TypeIdentical(tup_types[i], expected_tup_types[i]));
    }

    ASSERT_EQ(GetTypeName(tup), "(Int, Char, Float)");
    ASSERT_EQ(GetTypeTag(tup), TypeTag::Tuple);
}

