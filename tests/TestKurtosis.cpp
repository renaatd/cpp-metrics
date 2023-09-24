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
}

} // namespace
