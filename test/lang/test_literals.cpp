#include <gtest/gtest.h>

#include <komaru/lang/literal.hpp>
#include <komaru/util/std_extensions.hpp>

using namespace komaru::lang;
using namespace komaru::util;

TEST(Literals, Basic) {
    Literal nval = Literal::Number(42);
    Literal rval = Literal::Real(3.14);
    Literal cval = Literal::Char('m');
    Literal sval = Literal::String("mop");
    std::vector<Type> types = {Type::Var("a"), Type::Var("a"), Type::Char(), Type::String()};
    std::vector<Literal> values = {nval, rval, cval, sval};

    for (size_t i = 0; i < values.size(); ++i) {
        ASSERT_EQ(values[i].GetType(), types[i]);
    }

    ASSERT_TRUE(nval.Holds<int64_t>());
    ASSERT_FALSE(nval.Holds<double>());
    ASSERT_TRUE(rval.Holds<double>());
    ASSERT_FALSE(rval.Holds<int64_t>());
    ASSERT_TRUE(cval.Holds<char>());
    ASSERT_FALSE(cval.Holds<int64_t>());
    ASSERT_TRUE(sval.Holds<std::string>());
    ASSERT_FALSE(sval.Holds<char>());

    ASSERT_EQ(nval.GetVariant<int64_t>(), 42);
    ASSERT_EQ(rval.GetVariant<double>(), 3.14);
    ASSERT_EQ(cval.GetVariant<char>(), 'm');
    ASSERT_EQ(sval.GetVariant<std::string>(), "mop");
}
