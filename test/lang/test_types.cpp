#include <gtest/gtest.h>

#include <util/std_extensions.hpp>
#include <lang/type.hpp>

using namespace komaru::lang;
using namespace komaru::util;

TEST(Types, Atom) {
    std::vector<TypeTag> tags = {TypeTag::Int, TypeTag::Char, TypeTag::Float, TypeTag::Bool};
    std::vector<std::string> names = {"Int", "Char", "Float", "Bool"};
    std::vector<Type> types;

    for (auto tag : tags) {
        types.emplace_back(Type::FromTag(tag));
    }

    for (size_t i = 0; i < types.size(); ++i) {
        ASSERT_EQ(types[i], Type::FromTag(tags[i]));
        ASSERT_EQ(types[i].GetTag(), tags[i]);
        ASSERT_EQ(types[i].GetName(), names[i]);
        ASSERT_TRUE(types[i].Holds<AtomType>());
        ASSERT_FALSE(types[i].Holds<TupleType>());

        const auto& atom_type = types[i].GetVariant<AtomType>();
        ASSERT_EQ(atom_type.GetTag(), tags[i]);

        ASSERT_THROW(types[i].GetVariant<TupleType>(), std::exception);

        types[i].Visit(Overloaded{[&tags, i](const AtomType& atom_type) {
                                      ASSERT_EQ(atom_type.GetTag(), tags[i]);
                                  },
                                  [](const auto&) {
                                      FAIL();
                                  }});

        for (size_t j = 0; j < types.size(); ++j) {
            if (i == j) {
                continue;
            }

            ASSERT_NE(types[i], types[j]);
        }
    }
}

TEST(Types, Tuple) {
    Type tuple_ic1 = Type::TupleFromTags({TypeTag::Int, TypeTag::Char});
    Type tuple_ic2 = Type::Tuple({Type::FromTag(TypeTag::Int), Type::FromTag(TypeTag::Char)});
    Type tuple_ib = Type::TupleFromTags({TypeTag::Int, TypeTag::Bool});

    ASSERT_EQ(tuple_ic1, tuple_ic2);
    ASSERT_NE(tuple_ic1, tuple_ib);

    ASSERT_EQ(tuple_ic2.GetName(), tuple_ic1.GetVariant<TupleType>().GetName());
    ASSERT_EQ(tuple_ic2.GetName(), "(Int, Char)");
    ASSERT_TRUE(VecEq(tuple_ic1.GetVariant<TupleType>().GetTupleTypes(),
                      {Type::FromTag(TypeTag::Int), Type::FromTag(TypeTag::Char)}));
}
