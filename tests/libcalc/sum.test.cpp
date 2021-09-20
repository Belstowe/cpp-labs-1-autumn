#include <gtest/gtest.h>
#include <libcalc/sum.hpp>

TEST(summatorTest, accuracyCheck) {
	EXPECT_DOUBLE_EQ(Summator::sum(2, 40), 42);
	EXPECT_DOUBLE_EQ(Summator::sum(1, 2), 3);
	EXPECT_DOUBLE_EQ(Summator::sum(3.1415926535, 2.7182818284), 5.8598744819);
}
