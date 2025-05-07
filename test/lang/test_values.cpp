#include <gtest/gtest.h>

#include <komaru/lang/value.hpp>
#include <komaru/util/std_extensions.hpp>

using namespace komaru::lang;
using namespace komaru::util;

TEST(Values, Basic) {
    Value ival = Value::Int(42);
    Value cval = Value::Char('m');
    Value bval = Value::Bool(true);
    std::vector<Type> types = {Type::Int(), Type::Char(), Type::Bool()};
    std::vector<Value> values = {ival, cval, bval};

    for (size_t i = 0; i < values.size(); ++i) {
        ASSERT_EQ(values[i].GetType(), types[i]);
    }

    ASSERT_EQ(ival.GetVariant<AtomValue>().GetVariant<int32_t>(), 42);
    ASSERT_EQ(cval.GetVariant<AtomValue>().GetVariant<char>(), 'm');
    ASSERT_EQ(bval.GetVariant<AtomValue>().GetVariant<bool>(), true);

    ASSERT_TRUE(ival.GetVariant<AtomValue>().Holds<int32_t>());
    ASSERT_FALSE(ival.GetVariant<AtomValue>().Holds<char>());

    ival.Visit(Overloaded{[](const AtomValue& val) {
                              val.Visit(Overloaded{[](int32_t val) {
                                                       ASSERT_EQ(val, 42);
                                                   },
                                                   [](const auto&) {
                                                       FAIL();
                                                   }});
                          },
                          [](const auto&) {
                              FAIL();
                          }});
}
