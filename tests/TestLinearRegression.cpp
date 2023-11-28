#include "Metrics/LinearRegression.hpp"
#include "gtest/gtest.h"
#include <cmath>

namespace {

TEST(TestLinearRegression, noValue) {
    Metrics::LinearRegression<> dut;

    EXPECT_EQ(0, dut.count());
    EXPECT_TRUE(std::isnan(dut.slope()));
    EXPECT_TRUE(std::isnan(dut.intercept()));
    EXPECT_TRUE(std::isnan(dut.slope_through_origin()));
}

TEST(TestLinearRegression, singleValue) {
    Metrics::LinearRegression<> dut;

    dut.update(2.0, 6.0);

    EXPECT_EQ(1, dut.count());
    EXPECT_TRUE(std::isnan(dut.slope()));
    EXPECT_TRUE(std::isnan(dut.intercept()));
    EXPECT_DOUBLE_EQ(3.0, dut.slope_through_origin());
}

TEST(TestLinearRegression, twoValues) {
    Metrics::LinearRegression<> dut;

    dut.update(2.0, 5.0);
    dut.update(-2.0, 25.0);

    EXPECT_EQ(2, dut.count());
    EXPECT_DOUBLE_EQ(-5.0, dut.slope());
    EXPECT_DOUBLE_EQ(15.0, dut.intercept());
}

TEST(TestLinearRegression, accessStats) {
    Metrics::LinearRegression<> dut;

    dut.update(2.0, 5.0);
    dut.update(4.0, 25.0);

    EXPECT_EQ(2, dut.stats_x().count());
    EXPECT_EQ(2, dut.stats_y().count());

    EXPECT_DOUBLE_EQ(3.0, dut.stats_x().mean());
    EXPECT_DOUBLE_EQ(15.0, dut.stats_y().mean());
}

TEST(TestLinearRegression, addVaryingLength) {
    // example from
    // https://en.wikipedia.org/wiki/Simple_linear_regression#Numerical_example
    const std::vector<double> height = {1.47, 1.50, 1.52, 1.55, 1.57,
                                        1.60, 1.63, 1.65, 1.68, 1.70,
                                        1.73, 1.75, 1.78, 1.80, 1.83};
    const std::vector<double> mass = {52.21, 53.12, 54.48, 55.84, 57.20,
                                      58.57, 59.93, 61.29, 63.11, 64.47,
                                      66.28, 68.10, 69.92, 72.19, 74.46};

    Metrics::LinearRegression<> dut1;
    Metrics::LinearRegression<> dut2;
    for (int lenDut1 = 0; lenDut1 <= height.size(); lenDut1++) {
        dut1.reset();
        for (int i = 0; i < lenDut1; i++) {
            dut1.update(height[i], mass[i]);
        }
        dut2.reset();
        for (int i = lenDut1; i < height.size(); i++) {
            dut2.update(height[i], mass[i]);
        }
        dut1 += dut2;
        EXPECT_EQ(height.size(), dut1.count());
        EXPECT_NEAR(61.27219, dut1.slope(), 1e-5);
        EXPECT_NEAR(-39.06195, dut1.intercept(), 1e-5);
        // EXPECT_DOUBLE_EQ(0.9946, dut1.correlation());
    }
}

TEST(TestLinearRegression, slopeHighOffset) {
    constexpr double offset = 1e9;
    Metrics::LinearRegression<> dut;
    dut.update(4, offset + 4);
    dut.update(7, offset + 7);
    dut.update(13, offset + 13);
    dut.update(16, offset + 16);
    EXPECT_EQ(4, dut.count());
    EXPECT_DOUBLE_EQ(offset, dut.intercept());
    EXPECT_DOUBLE_EQ(1.0, dut.slope());
}

TEST(TestLinearRegression, toString) {
    Metrics::LinearRegression<> dut;

    EXPECT_EQ(0, dut.toString(1).find("count(0) slope(nan) intercept(nan)"));

    dut.update(2.0, 5.0);
    dut.update(-2.0, 25.0);

    EXPECT_EQ(0, dut.toString(1).find("count(2) slope(-5.0) intercept(15.0)"));
}
} // namespace
