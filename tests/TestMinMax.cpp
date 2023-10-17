#include "Metrics/MinMax.hpp"
#include "gtest/gtest.h"
#include <cmath>

namespace {

TEST(TestMinMax, singleValue) {
    Metrics::MinMax<> dut;

    EXPECT_TRUE(std::isnan(dut.min()));
    EXPECT_TRUE(std::isnan(dut.max()));

    dut.update(-1);
    EXPECT_EQ(-1, dut.min());
    EXPECT_EQ(-1, dut.max());
}

TEST(TestMinMax, threeValues) {
    Metrics::MinMax<> dut;

    dut.update(1);
    dut.update(2);
    dut.update(3);
    EXPECT_EQ(1, dut.min());
    EXPECT_EQ(3, dut.max());
    EXPECT_EQ(3, dut.count());
}

TEST(TestMinMax, reset) {
    Metrics::MinMax<> dut;

    dut.update(-1);
    dut.reset();
    EXPECT_TRUE(std::isnan(dut.min()));
    EXPECT_TRUE(std::isnan(dut.max()));
    EXPECT_EQ(0, dut.count());
    dut.update(2);
    EXPECT_EQ(2, dut.min());
    EXPECT_EQ(2, dut.max());
    EXPECT_EQ(1, dut.count());
}

TEST(TestMinMax, operator_compound_plus) {
    Metrics::MinMax<> dut1;
    Metrics::MinMax<> dut2;

    // adding 2 empty DUTs
    dut1 += dut2;
    EXPECT_TRUE(std::isnan(dut1.min()));
    EXPECT_TRUE(std::isnan(dut1.max()));

    // adding empty DUT to non-empty DUT
    dut1.update(-1);
    dut1.update(-3);
    dut1 += dut2;
    EXPECT_EQ(-3, dut1.min());
    EXPECT_EQ(-1, dut1.max());
    EXPECT_EQ(2, dut1.count());

    // adding non-empty DUT to empty DUT
    dut2 += dut1;
    EXPECT_EQ(-3, dut2.min());
    EXPECT_EQ(-1, dut2.max());
    EXPECT_EQ(2, dut2.count());

    // adding 2 non-empty DUTs
    dut2.reset();
    dut2.update(-5);
    dut2.update(-7);
    dut1 += dut2;
    EXPECT_EQ(-7, dut1.min());
    EXPECT_EQ(-1, dut1.max());
    EXPECT_EQ(4, dut1.count());

    // adding to self - should not deadlock
    dut1 += dut1;
    EXPECT_EQ(-7, dut1.min());
    EXPECT_EQ(-1, dut1.max());
    EXPECT_EQ(8, dut1.count());
}

TEST(TestMinMax, operator_plus) {
    Metrics::MinMax<> dut1;
    Metrics::MinMax<> dut2;

    // adding 2 non-empty DUTs
    dut1.update(-1);
    dut1.update(-3);

    dut2.update(-5);
    dut2.update(-7);

    auto dut3 = dut1 + dut2;
    EXPECT_EQ(-7, dut3.min());
    EXPECT_EQ(-1, dut3.max());
    EXPECT_EQ(4, dut3.count());

    // no change in dut1 and dut2
    EXPECT_EQ(2, dut1.count());
    EXPECT_EQ(2, dut2.count());
}

TEST(TestMinMax, toString) {
    Metrics::MinMax<> dut;

    EXPECT_EQ("count(0) min(nan) max(nan)", dut.toString(1));
    dut.update(1);
    dut.update(2);
    dut.update(3);
    EXPECT_EQ("count(3) min(1.0) max(3.0)", dut.toString(1));
}

TEST(TestMinMax, constructors) {
    Metrics::MinMax<> dut1;
    dut1.update(1);

    Metrics::MinMax<> dut2(dut1);
    EXPECT_EQ(1, dut1.count());
    EXPECT_EQ(1, dut2.count());
}

TEST(TestMinMax, assignments) {
    Metrics::MinMax<> dut1;
    dut1.update(1);

    Metrics::MinMax<> dut2;
    dut2 = dut1;
    EXPECT_EQ(1, dut1.count());
    EXPECT_EQ(1, dut2.count());
}
} // namespace
