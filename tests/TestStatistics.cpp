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

TEST(TestStatistics, variance) {
    Metrics::Statistics<> dut;

    EXPECT_TRUE(std::isnan(dut.variance()));
    EXPECT_TRUE(std::isnan(dut.stddev()));
    EXPECT_TRUE(std::isnan(dut.sample_variance()));
    EXPECT_TRUE(std::isnan(dut.sample_stddev()));

    dut.update(10);
    EXPECT_DOUBLE_EQ(0.0, dut.variance());
    EXPECT_DOUBLE_EQ(0.0, dut.stddev());
    EXPECT_TRUE(std::isnan(dut.sample_variance()));
    EXPECT_TRUE(std::isnan(dut.sample_stddev()));

    dut.update(12);
    EXPECT_DOUBLE_EQ(1.0, dut.variance());
    EXPECT_DOUBLE_EQ(1.0, dut.stddev());
    EXPECT_DOUBLE_EQ(2.0, dut.sample_variance());
    EXPECT_DOUBLE_EQ(sqrt(2.0), dut.sample_stddev());

    dut.update(12);
    dut.update(14);
    EXPECT_EQ(10, dut.min());
    EXPECT_EQ(12, dut.mean());
    EXPECT_EQ(14, dut.max());
    EXPECT_EQ(4, dut.count());
    EXPECT_DOUBLE_EQ(2.0, dut.variance());
    EXPECT_DOUBLE_EQ(sqrt(2.0), dut.stddev());
    EXPECT_DOUBLE_EQ(2.0 * 4.0 / 3.0, dut.sample_variance());
    EXPECT_DOUBLE_EQ(sqrt(2.0 * 4.0 / 3.0), dut.sample_stddev());
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
