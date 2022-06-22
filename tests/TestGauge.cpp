#include "Metrics/Gauge.hpp"
#include "gtest/gtest.h"

namespace {

TEST(TestGauge, update) {
    Metrics::Gauge<> dut;

    EXPECT_EQ(0, dut.value());
    dut.update(-1);
    EXPECT_EQ(-1, dut.value());
}

TEST(TestGauge, reset) {
    Metrics::Gauge<> dut;

    dut.update(-1);
    dut.reset();
    EXPECT_EQ(0, dut.value());
}

TEST(TestGauge, toString) {
    Metrics::Gauge<> dut;

    dut.update(123.12);
    EXPECT_EQ("123.1", dut.toString(1));
}

TEST(TestGauge, withFloat) {
    Metrics::Gauge<float> dut;

    dut.update(123.12);
    EXPECT_EQ(123.12f, dut.value());
}
} // namespace
