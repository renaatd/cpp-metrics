#include "Metrics/Histogram.hpp"
#include "Metrics/SlidingWindowReservoir.hpp"
#include "gtest/gtest.h"
#include <cmath>

namespace {

TEST(TestHistogram, noDataNoError) {
    Metrics::Histogram<Metrics::SlidingWindowReservoir<>> dut(1, true, 2);

    EXPECT_EQ(0, dut.toString().find(
                     "count(0), min(0), Q25(0), Q50(0), Q75(0), max(0)"));
}

TEST(TestHistogram, snapshotCorrect) {
    Metrics::Histogram<Metrics::SlidingWindowReservoir<>> dut(2, true, 2);

    dut.update(-4);
    dut.update(8);
    auto snapshot = dut.getSnapshot();

    EXPECT_EQ(2, snapshot.size());
    EXPECT_EQ(-4, snapshot.getValue(0));
    EXPECT_EQ(8, snapshot.getValue(1));
    // std::cout << dut.toString();
}

TEST(TestHistogram, reset) {
    Metrics::Histogram<Metrics::SlidingWindowReservoir<>> dut(3);

    dut.update(1);
    dut.reset();
    dut.update(2);
    dut.update(4);
    EXPECT_EQ(2, dut.getSnapshot().size());
}

TEST(TestHistogram, toString) {
    Metrics::Histogram<Metrics::SlidingWindowReservoir<>> dut(4, false, 4);

    // feed with values 0-4, and we ask 4 output bins -> each bin will be 1 wide
    // bin 0 will have 2 values, bins 1 and 3 have 1 value, bin 2 is empty
    dut.update(0);
    dut.update(0);
    dut.update(1.5);
    dut.update(4);

    // std::cout << dut.toString();

    auto bins = dut.getSnapshot().getBins(4, 0, 4);
    EXPECT_EQ(2, bins[0]);
    EXPECT_EQ(1, bins[1]);
    EXPECT_EQ(0, bins[2]);
    EXPECT_EQ(1, bins[3]);
}
} // namespace
