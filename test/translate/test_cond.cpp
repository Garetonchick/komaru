#include <gtest/gtest.h>

#include <komaru/translate/common/cond.hpp>

using komaru::translate::common::Cond;

TEST(CppCond, Basic) {
    Cond truth;
    Cond x1 = Cond(0);
    Cond x2 = Cond(1);
    Cond x3 = Cond(2);
    Cond x4 = Cond(3);

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
