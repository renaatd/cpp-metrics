#include "Metrics/Kurtosis.hpp"
#include "gtest/gtest.h"
#include <cmath>

namespace {

TEST(TestKurtosis, singleValue) {
    Metrics::Kurtosis<> dut;
    const std::vector<double> inputs{0, 3, 4, 1, 2, 3, 0, 2, 1, 3,
                                     2, 0, 2, 2, 3, 2, 5, 2, 3, 999};

    for (const auto x : inputs) {
        dut.update(x);
    }

    EXPECT_NEAR(15.05, dut.excess_kurtosis(), 1e-2);
}

TEST(TestKurtosis, varianceHighOffset) {
    // Example from
    // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Example
    // - demo correct behavior when high offset
    constexpr double offset = 1e9;
    Metrics::Kurtosis<> dut;
    dut.update(offset + 4);
    dut.update(offset + 7);
    dut.update(offset + 13);
    dut.update(offset + 16);
    EXPECT_EQ(4, dut.count());
    EXPECT_DOUBLE_EQ(30.0, dut.sample_variance());
    EXPECT_DOUBLE_EQ(sqrt(30.0), dut.sample_stddev());
}

TEST(TestKurtosis, reset) {
    Metrics::Kurtosis<> dut;

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

TEST(TestKurtosis, rmsFirstSample) {
    Metrics::Kurtosis<> dut;

    // RMS of 0 samples is NAN
    EXPECT_TRUE(std::isnan(dut.rms()));

    // RMS of 1 sample is the absolute value of the sample
    dut.update(-5.0);
    EXPECT_DOUBLE_EQ(5.0, dut.rms());
}

TEST(TestKurtosis, rms) {
    constexpr int LOOPS = 10;
    Metrics::Kurtosis<> dut;

    // signal with DC value 3 + square wave amplitude 4 has RMS value 5
    for (int i = 0; i < LOOPS; i++) {
        dut.update(3 + 4);
        dut.update(3 - 4);
    }

    EXPECT_DOUBLE_EQ(5.0, dut.rms());
}

} // namespace
