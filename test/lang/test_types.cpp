#include <gtest/gtest.h>

#include <komaru/util/std_extensions.hpp>
#include <komaru/lang/type.hpp>

using namespace komaru::lang;
using namespace komaru::util;

TEST(Types, Atom) {
    std::vector<std::string> names = {"Int", "Char", "Float", "Bool"};
    std::vector<Type> types = {Type::Int(), Type::Char(), Type::Float(), Type::Bool()};

    for (size_t i = 0; i < types.size(); ++i) {
        ASSERT_EQ(types[i].GetName(), names[i]);
        ASSERT_TRUE(types[i].Holds<CommonType>());
        ASSERT_FALSE(types[i].Holds<TupleType>());

        const auto& common_type = types[i].GetVariant<CommonType>();
        ASSERT_EQ(common_type.GetMainName(), names[i]);

        ASSERT_THROW(types[i].GetVariant<TupleType>(), std::exception);

        types[i].Visit(Overloaded{[&names, i](const CommonType& common_type) {
                                      ASSERT_EQ(common_type.GetMainName(), names[i]);
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
    Type tuple_ic1 = Type::TupleFromSimples({"Int", "Char"});
    Type tuple_ic2 = Type::Tuple({Type::Int(), Type::Char()});
    Type tuple_ib = Type::TupleFromSimples({"Int", "Bool"});

    ASSERT_EQ(tuple_ic1, tuple_ic2);
    ASSERT_NE(tuple_ic1, tuple_ib);

    ASSERT_EQ(tuple_ic2.GetName(), tuple_ic1.GetVariant<TupleType>().GetName());
    ASSERT_EQ(tuple_ic2.GetName(), "Int x Char");
    ASSERT_TRUE(
        VecEq(tuple_ic1.GetVariant<TupleType>().GetTupleTypes(), {Type::Int(), Type::Char()}));
}
