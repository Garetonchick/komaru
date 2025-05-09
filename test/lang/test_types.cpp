#include <gtest/gtest.h>

#include <komaru/util/std_extensions.hpp>
#include <komaru/lang/type.hpp>
#include <komaru/lang/morphism.hpp>
using namespace komaru::lang;
using namespace komaru::util;

TEST(Types, Atom) {
    std::vector<std::string> names = {"Int", "Char", "Float", "Bool"};
    std::vector<Type> types = {Type::Int(), Type::Char(), Type::Float(), Type::Bool()};

    for (size_t i = 0; i < types.size(); ++i) {
        ASSERT_EQ(types[i].ToString(), names[i]);
        ASSERT_TRUE(types[i].Holds<CommonType>());
        ASSERT_FALSE(types[i].Holds<TupleType>());

        const auto& common_type = types[i].GetVariant<CommonType>();
        ASSERT_EQ(common_type.GetName(), names[i]);

        ASSERT_THROW(types[i].GetVariant<TupleType>(), std::exception);

        types[i].Visit(Overloaded{[&names, i](const CommonType& common_type) {
                                      ASSERT_EQ(common_type.GetName(), names[i]);
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

    ASSERT_EQ(tuple_ic2.ToString(), tuple_ic1.GetVariant<TupleType>().ToString());
    ASSERT_EQ(tuple_ic2.ToString(), "Int x Char");
    ASSERT_TRUE(
        VecEq(tuple_ic1.GetVariant<TupleType>().GetTupleTypes(), {Type::Int(), Type::Char()}));
}

TEST(Types, Function) {
    Type func_ic = Type::Function(Type::Int(), Type::Char());

    ASSERT_EQ(func_ic.ToString(), "Int -> Char");
    ASSERT_TRUE(func_ic.Holds<FunctionType>());

    ASSERT_EQ(func_ic.GetVariant<FunctionType>().Source(), Type::Int());
    ASSERT_EQ(func_ic.GetVariant<FunctionType>().Target(), Type::Char());

    Type func_icc = Type::FunctionChain(std::vector<Type>{Type::Int(), Type::Char(), Type::Char()});
    ASSERT_EQ(func_icc.ToString(), "Int -> Char -> Char");
    ASSERT_TRUE(func_icc.Holds<FunctionType>());

    ASSERT_EQ(func_icc.GetVariant<FunctionType>().Source(), Type::Int());
    ASSERT_EQ(func_icc.GetVariant<FunctionType>().Target(),
              Type::Function(Type::Char(), Type::Char()));
}

TEST(Types, List) {
    Type list_i = Type::List(Type::Int());
    ASSERT_EQ(list_i.ToString(), "[Int]");
    ASSERT_TRUE(list_i.Holds<ListType>());
    ASSERT_EQ(list_i.GetVariant<ListType>().Inner(), Type::Int());
}

TEST(Types, SimpleDeduction) {
    Type func_ic = Type::Function(Type::Var("a"), Type::Char());
    Type arg_i = Type::Int();
    auto deduced = TryDeduceTypes(func_ic, arg_i);
    ASSERT_TRUE(deduced.has_value());
    ASSERT_EQ(deduced.value(), Type::Function(Type::Int(), Type::Char()));

    Type plus_t = Morphism::Plus()->GetType();
    auto maybe_deduced_type = TryDeduceTypes(plus_t, {{0, Type::Int()}, {1, Type::Int()}});
    ASSERT_TRUE(maybe_deduced_type.has_value());
    ASSERT_EQ(maybe_deduced_type.value(),
              Type::FunctionChain({Type::Int(), Type::Int(), Type::Int()}));
}

TEST(Types, SimpleSubstitution) {
    Type plus_t = Morphism::Plus()->GetType();
    auto maybe_deduced_type = TryMakeSubstitution(plus_t, {{0, Type::Int()}, {1, Type::Int()}});
    ASSERT_TRUE(maybe_deduced_type.has_value());
    ASSERT_EQ(maybe_deduced_type.value(), Type::Int());
}

TEST(Types, MatchDoesNotLoseInformation) {
    auto param_type = Type::Int();
    auto arg_type = Type::Var("a");
    auto maybe_match_map = TryMatchTypes(param_type, arg_type);
    ASSERT_TRUE(maybe_match_map.has_value());

    auto match_map = maybe_match_map.value();

    ASSERT_TRUE(match_map.empty());

    auto deduced_type = ApplyMatchMap(param_type, match_map);
    ASSERT_EQ(deduced_type.ToString(), param_type.ToString());
}
