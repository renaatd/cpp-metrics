#include "Metrics/Snapshot.hpp"
#include "gtest/gtest.h"

namespace {
const std::vector<double> t1 = {100, 150, 200};
const std::vector<double> t2 = {150, 200, 100};
const std::vector<double> t3 = {200, 100, 150};

TEST(TestSnapshot, sorted) {
    for (auto &inputVector : {t1, t2, t3}) {
        auto dut =
            Metrics::Snapshot<>(inputVector.cbegin(), inputVector.cend());
        EXPECT_EQ(3, dut.size());
        EXPECT_EQ(100, dut.values()[0]);
        EXPECT_EQ(150, dut.values()[1]);
        EXPECT_EQ(200, dut.values()[2]);
    }
}

TEST(TestSnapshot, quantileInRangeOddLength) {
    Metrics::Snapshot<> dut{t1.cbegin(), t1.cend()};

    EXPECT_EQ(100, dut.getValue(0.0));
    EXPECT_EQ(125, dut.getValue(0.25));
    EXPECT_EQ(150, dut.getValue(0.50));
    EXPECT_EQ(175, dut.getValue(0.75));
    EXPECT_EQ(200, dut.getValue(1.00));
}

TEST(TestSnapshot, quantileInRangeEvenLength) {
    const std::vector<double> t = {200, 400, 600, 800};
    Metrics::Snapshot<> dut{t.cbegin(), t.cend()};

    EXPECT_EQ(200, dut.getValue(0.0));
    EXPECT_EQ(350, dut.getValue(0.25));
    EXPECT_EQ(500, dut.getValue(0.50));
    EXPECT_EQ(650, dut.getValue(0.75));
    EXPECT_EQ(800, dut.getValue(1.00));
}

TEST(TestSnapshot, quantileOutOfRange) {
    Metrics::Snapshot<> dut{t1.cbegin(), t1.cend()};

    EXPECT_THROW(dut.getValue(-1e-9), std::invalid_argument);
    EXPECT_THROW(dut.getValue(1 + 1e-9), std::invalid_argument);
}

TEST(TestSnapshot, binsCorrect) {
    Metrics::Snapshot<> dut{t1.cbegin(), t1.cend()};

    // bins 125..225, 225..325-> everything in bin 0
    auto bins = dut.getBins(2, 125, 200);
    EXPECT_EQ(3, bins[0]);
    EXPECT_EQ(0, bins[1]);

    // bins 50..100, 100..150 -> everything in bin 1
    bins = dut.getBins(2, 50, 100);
    EXPECT_EQ(0, bins[0]);
    EXPECT_EQ(3, bins[1]);

    // bins 75..175, 175..275-> two in bin 0, one in bin 1
    bins = dut.getBins(2, 75, 200);
    EXPECT_EQ(2, bins[0]);
    EXPECT_EQ(1, bins[1]);
}

} // namespace
