#include "Metrics/Statistics.hpp"
#include "gtest/gtest.h"
#include <cmath>

namespace {

TEST(TestStatistics, singleValue) {
    Metrics::Statistics<> dut;

    EXPECT_TRUE(std::isnan(dut.mean()));
    dut.update(-1);
    EXPECT_EQ(-1, dut.min());
    EXPECT_EQ(-1, dut.mean());
    EXPECT_EQ(-1, dut.max());
}

TEST(TestStatistics, threeValues) {
    Metrics::Statistics<> dut;

    dut.update(1);
    dut.update(2);
    dut.update(3);
    EXPECT_EQ(1, dut.min());
    EXPECT_EQ(2, dut.mean());
    EXPECT_EQ(3, dut.max());
    EXPECT_EQ(3, dut.count());
}

TEST(TestStatistics, reset) {
    Metrics::Statistics<> dut;

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

TEST(TestStatistics, toString) {
    Metrics::Statistics<> dut;

    EXPECT_EQ(0, dut.toString(1).find("count(0) min(nan) mean(nan) max(nan)"));
    dut.update(1);
    dut.update(2);
    dut.update(3);
    EXPECT_EQ(0, dut.toString(1).find("count(3) min(1.0) mean(2.0) max(3.0)"));
}
} // namespace
