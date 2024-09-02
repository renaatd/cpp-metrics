#include "Metrics/Variance.hpp"
#include "gtest/gtest.h"
#include <cmath>

namespace {

TEST(TestVariance, singleValue) {
    Metrics::Variance<> dut;

    EXPECT_TRUE(std::isnan(dut.mean()));
    EXPECT_EQ(0, dut.mean0());

    dut.update(-1);
    EXPECT_EQ(-1, dut.min());
    EXPECT_EQ(-1, dut.mean());
    EXPECT_EQ(-1, dut.mean0());
    EXPECT_EQ(-1, dut.max());
    EXPECT_EQ(0, dut.m2());
}

TEST(TestVariance, threeValues) {
    Metrics::Variance<> dut;

    dut.update(1);
    dut.update(2);
    dut.update(3);
    EXPECT_EQ(1, dut.min());
    EXPECT_EQ(2, dut.mean());
    EXPECT_EQ(3, dut.max());
    EXPECT_EQ(3, dut.count());
    EXPECT_EQ(2, dut.m2());
}

TEST(TestVariance, threeValuesCompoundPlus) {
    Metrics::Variance<> dut1;
    dut1.update(1);
    dut1.update(2);
    dut1.update(3);

    // add empty DUT to non-empty DUT
    Metrics::Variance<> dut2;
    dut1 += dut2;

    EXPECT_EQ(1, dut1.min());
    EXPECT_EQ(2, dut1.mean());
    EXPECT_EQ(3, dut1.max());
    EXPECT_EQ(3, dut1.count());
    EXPECT_EQ(2, dut1.m2());
    EXPECT_EQ(0, dut2.count());

    // add non-empty DUT to empty DUT
    dut2 += dut1;

    EXPECT_EQ(1, dut2.min());
    EXPECT_EQ(2, dut2.mean());
    EXPECT_EQ(3, dut2.max());
    EXPECT_EQ(3, dut2.count());
    EXPECT_EQ(2, dut2.m2());
    EXPECT_EQ(3, dut1.count());

    // add non-empty DUT to non-empty DUT
    dut1.reset();
    dut2.reset();
    dut1.update(1);
    dut2.update(2);
    dut2.update(3);

    dut1 += dut2;

    EXPECT_EQ(1, dut1.min());
    EXPECT_EQ(2, dut1.mean());
    EXPECT_EQ(3, dut1.max());
    EXPECT_EQ(3, dut1.count());
    EXPECT_EQ(2, dut1.m2());
    EXPECT_EQ(2, dut2.count());

    // should not deadlock
    dut1 += dut1;
    EXPECT_EQ(6, dut1.count());
}

TEST(TestVariance, threeValuesPlus) {
    Metrics::Variance<> dut1;
    Metrics::Variance<> dut2;

    dut1.update(1);
    dut1.update(2);
    dut2.update(3);

    auto dut = dut1 + dut2;
    EXPECT_EQ(1, dut.min());
    EXPECT_EQ(2, dut.mean());
    EXPECT_EQ(3, dut.max());
    EXPECT_EQ(3, dut.count());
    EXPECT_EQ(2, dut.m2());
}

TEST(TestVariance, variance) {
    Metrics::Variance<> dut;

    EXPECT_TRUE(std::isnan(dut.variance()));
    EXPECT_TRUE(std::isnan(dut.stddev()));
    EXPECT_TRUE(std::isnan(dut.sample_variance()));
    EXPECT_TRUE(std::isnan(dut.sample_stddev()));
    EXPECT_EQ(0, dut.m2());

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

TEST(TestVariance, varianceHighOffset) {
    // Example from
    // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Example
    // - demo correct behavior when high offset
    constexpr double offset = 1e9;
    Metrics::Variance<> dut;
    dut.update(offset + 4);
    dut.update(offset + 7);
    dut.update(offset + 13);
    dut.update(offset + 16);
    EXPECT_EQ(4, dut.count());
    EXPECT_DOUBLE_EQ(30.0, dut.sample_variance());
}

TEST(TestVariance, varianceHighOffsetCompoundPlus) {
    // Example from
    // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Example
    // - demo correct behavior when high offset
    constexpr double offset = 1e9;
    Metrics::Variance<> dut1;
    dut1.update(offset + 4);
    dut1.update(offset + 7);
    Metrics::Variance<> dut2;
    dut2.update(offset + 13);
    dut2.update(offset + 16);
    dut1 += dut2;
    EXPECT_EQ(2, dut2.count());
    EXPECT_EQ(4, dut1.count());
    EXPECT_DOUBLE_EQ(30.0, dut1.sample_variance());
}

TEST(TestVariance, reset) {
    Metrics::Variance<> dut;

    dut.update(-1);
    dut.reset();
    EXPECT_TRUE(std::isnan(dut.min()));
    EXPECT_TRUE(std::isnan(dut.mean()));
    EXPECT_EQ(0, dut.mean0());
    EXPECT_TRUE(std::isnan(dut.max()));
    EXPECT_EQ(0, dut.count());
    dut.update(2);
    EXPECT_EQ(2, dut.min());
    EXPECT_EQ(2, dut.mean());
    EXPECT_EQ(2, dut.mean0());
    EXPECT_EQ(2, dut.max());
    EXPECT_EQ(1, dut.count());
}

TEST(TestVariance, rmsFirstSample) {
    Metrics::Variance<> dut;

    // RMS of 0 samples is NAN
    EXPECT_TRUE(std::isnan(dut.rms()));

    // RMS of 1 sample is the absolute value of the sample
    dut.update(-5.0);
    EXPECT_DOUBLE_EQ(5.0, dut.rms());
}

TEST(TestVariance, rms) {
    constexpr int LOOPS = 10;
    Metrics::Variance<> dut;

    // signal with DC value 3 + square wave amplitude 4 has RMS value 5
    for (int i = 0; i < LOOPS; i++) {
        dut.update(3 + 4);
        dut.update(3 - 4);
    }

    EXPECT_DOUBLE_EQ(5.0, dut.rms());
}

TEST(TestVariance, toString) {
    Metrics::Variance<> dut;

    EXPECT_EQ(0, dut.toString(1).find("count(0) min(nan) mean(nan) max(nan)"));
    dut.update(1);
    dut.update(2);
    dut.update(3);
    EXPECT_EQ(0, dut.toString(1).find("count(3) min(1.0) mean(2.0) max(3.0)"));
}

TEST(TestVariance, constructors) {
    Metrics::Variance<> dut1;
    dut1.update(1);

    Metrics::Variance<> dut2(dut1);
    EXPECT_EQ(1, dut1.count());
    EXPECT_EQ(1, dut2.count());
}

TEST(TestVariance, assignments) {
    Metrics::Variance<> dut1;
    dut1.update(1);
    dut1.update(3);

    Metrics::Variance<> dut2;
    dut2 = dut1;
    EXPECT_EQ(2, dut1.mean());
    EXPECT_EQ(2, dut2.mean());

    dut2 = dut2;
    EXPECT_EQ(2, dut2.mean());
}

TEST(TestVariance, addEmpty) {
    Metrics::Variance<> dut1;
    Metrics::Variance<> dut2;

    // add empty DUT to empty DUT
    dut1 += dut2;

    EXPECT_EQ(0, dut1.count());
    EXPECT_EQ(0, dut2.count());

    dut1.update(1);
    EXPECT_EQ(1, dut1.mean());
}
} // namespace
