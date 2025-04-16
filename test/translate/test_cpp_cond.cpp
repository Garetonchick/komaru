#include <gtest/gtest.h>

#include <komaru/translate/cpp/cpp_cond.hpp>

using komaru::translate::cpp::CppCond;

TEST(CppCond, Basic) {
    CppCond truth;
    CppCond x1 = CppCond(0);
    CppCond x2 = CppCond(1);
    CppCond x3 = CppCond(2);
    CppCond x4 = CppCond(3);

    ASSERT_FALSE(truth.DoesImply(x1));
    ASSERT_FALSE(truth.DoesImply(x2));
    ASSERT_FALSE(truth.DoesImply(x1 & x2));
    ASSERT_FALSE(truth.DoesImply(x1 | x2));
    ASSERT_FALSE(truth.DoesImply((x1 & x4) | (x3 & x2)));
    ASSERT_FALSE(x1.DoesImply(x1 & x2));
    ASSERT_FALSE(x2.DoesImply(x1 & x2));
    ASSERT_FALSE((x1 & x2).DoesImply((x1 | x2) & x3));

    ASSERT_TRUE(truth.DoesImply(truth));
    ASSERT_TRUE(truth.DoesImply(truth & truth));
    ASSERT_TRUE(truth.DoesImply(truth | truth));
    ASSERT_TRUE((truth & truth).DoesImply(truth));
    ASSERT_TRUE((truth | truth).DoesImply(truth));
    ASSERT_TRUE((truth | truth).DoesImply(truth | truth));
    ASSERT_TRUE((truth | truth).DoesImply(truth & truth));
    ASSERT_TRUE((truth & truth).DoesImply(truth & truth));
    ASSERT_TRUE((truth & truth).DoesImply(truth | truth));
    ASSERT_TRUE(truth.DoesImply((x1 & x4) | truth));
    ASSERT_TRUE(truth.DoesImply(truth | (x3 & x2)));
    ASSERT_TRUE((x1 & x2).DoesImply(x2 & x1));
    ASSERT_TRUE((x1 | x2).DoesImply(x2 | x1));
    ASSERT_TRUE(x1.DoesImply(x2 | x1));
    ASSERT_TRUE(x2.DoesImply(x2 | x1));
    ASSERT_TRUE((x1 & x2).DoesImply(x1));
    ASSERT_TRUE((x1 & x2).DoesImply(x2));
    ASSERT_TRUE((x1 & x2).DoesImply(x1 | x2));
}
