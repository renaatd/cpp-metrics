#include "Metrics/MinMeanMax.hpp"
#include "gtest/gtest.h"
#include <cmath>

namespace {

TEST(TestMinMeanMax, singleValue) {
    Metrics::MinMeanMax<> dut;

    EXPECT_TRUE(std::isnan(dut.mean()));
    dut.update(-1);
    EXPECT_EQ(-1, dut.min());
    EXPECT_EQ(-1, dut.mean());
    EXPECT_EQ(-1, dut.max());
}

TEST(TestMinMeanMax, threeValues) {
    Metrics::MinMeanMax<> dut;

    dut.update(1);
    dut.update(2);
    dut.update(3);
    EXPECT_EQ(1, dut.min());
    EXPECT_EQ(2, dut.mean());
    EXPECT_EQ(3, dut.max());
    EXPECT_EQ(3, dut.count());
}

TEST(TestMinMeanMax, reset) {
    Metrics::MinMeanMax<> dut;

    dut.update(-1);
    dut.reset();
    EXPECT_TRUE(std::isnan(dut.min()));
    EXPECT_TRUE(std::isnan(dut.mean()));
    EXPECT_TRUE(std::isnan(dut.max()));
    EXPECT_EQ(0, dut.count());
    dut.update(2);
    EXPECT_EQ(2, dut.min());
    EXPECT_EQ(2, dut.mean());
    EXPECT_EQ(2, dut.max());
    EXPECT_EQ(1, dut.count());
}

TEST(TestMinMeanMax, operator_compound_plus) {
    Metrics::MinMeanMax<> dut1;
    Metrics::MinMeanMax<> dut2;

    // adding 2 empty DUTs
    dut1 += dut2;
    EXPECT_TRUE(std::isnan(dut1.min()));
    EXPECT_TRUE(std::isnan(dut1.mean()));
    EXPECT_TRUE(std::isnan(dut1.max()));

    // adding empty DUT to non-empty DUT
    dut1.update(-1);
    dut1.update(-3);
    dut1 += dut2;
    EXPECT_EQ(-3, dut1.min());
    EXPECT_EQ(-2, dut1.mean());
    EXPECT_EQ(-1, dut1.max());
    EXPECT_EQ(2, dut1.count());

    // adding non-empty DUT to empty DUT
    dut2 += dut1;
    EXPECT_EQ(-3, dut2.min());
    EXPECT_EQ(-2, dut2.mean());
    EXPECT_EQ(-1, dut2.max());
    EXPECT_EQ(2, dut2.count());

    // adding 2 non-empty DUTs
    dut2.reset();
    dut2.update(-5);
    dut2.update(-7);
    dut1 += dut2;
    EXPECT_EQ(-7, dut1.min());
    EXPECT_EQ(-4, dut1.mean());
    EXPECT_EQ(-1, dut1.max());
    EXPECT_EQ(4, dut1.count());
}

TEST(TestMinMeanMax, toString) {
    Metrics::MinMeanMax<> dut;

    EXPECT_EQ("count(0) min(nan) mean(nan) max(nan)", dut.toString(1));
    dut.update(1);
    dut.update(2);
    dut.update(3);
    EXPECT_EQ("count(3) min(1.0) mean(2.0) max(3.0)", dut.toString(1));
}
} // namespace
