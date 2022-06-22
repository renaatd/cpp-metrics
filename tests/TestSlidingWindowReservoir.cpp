#include "Metrics/SlidingWindowReservoir.hpp"
#include "gtest/gtest.h"

namespace {

TEST(TestSlidingWindowReservoir, firstAllStored) {
    Metrics::SlidingWindowReservoir<> dut{3};

    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(3, dut.size());
        EXPECT_EQ(i, dut.samples());
        dut.update(10 + i);
    }
    auto values = dut.getSnapshot().values();
    EXPECT_EQ(10, values[0]);
    EXPECT_EQ(11, values[1]);
    EXPECT_EQ(12, values[2]);
}

TEST(TestSlidingWindowReservoir, storedMore) {
    constexpr int SAMPLES_ADDED = 1000;
    Metrics::SlidingWindowReservoir<> dut{3};

    for (int i = 0; i < SAMPLES_ADDED; i++) {
        EXPECT_EQ(3, dut.size());
        if (i >= 3) {
            EXPECT_EQ(3, dut.samples());
        }
        dut.update(10 + i);
    }
    auto values = dut.getSnapshot().values();
    EXPECT_EQ(3, values.size());
    EXPECT_EQ(SAMPLES_ADDED + 10 - 3, values[0]);
    EXPECT_EQ(SAMPLES_ADDED + 10 - 2, values[1]);
    EXPECT_EQ(SAMPLES_ADDED + 10 - 1, values[2]);
}

TEST(TestSlidingWindowReservoir, reset) {
    Metrics::SlidingWindowReservoir<> dut{3};

    dut.update(-1);
    dut.reset();
    EXPECT_EQ(0, dut.samples());
    dut.update(2);
    EXPECT_EQ(1, dut.samples());
    auto snapshot = dut.getSnapshot();
    EXPECT_EQ(1, snapshot.size());
    EXPECT_EQ(2, snapshot.values()[0]);
}
} // namespace
