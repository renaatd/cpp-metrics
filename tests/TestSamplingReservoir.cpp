#include "Metrics/SamplingReservoir.hpp"
#include "gtest/gtest.h"

namespace {

TEST(TestSamplingReservoir, firstAllStored) {
    Metrics::SamplingReservoir<> dut{3};

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

TEST(TestSamplingReservoir, storedMore) {
    constexpr int SAMPLES_ADDED = 1000;
    Metrics::SamplingReservoir<> dut{3};

    for (int i = 0; i < SAMPLES_ADDED; i++) {
        EXPECT_EQ(3, dut.size());
        if (i >= 3) {
            EXPECT_EQ(3, dut.samples());
        }
        EXPECT_EQ(i, dut.count());
        dut.update(10 + i);
    }
    auto values = dut.getSnapshot().values();
    EXPECT_EQ(3, values.size());
#if 0
  std::cout << "Values: ";
  for (auto x: values) {
    std::cout << x << " ";
  }
  std::cout << std::endl;
#endif
}

TEST(TestSamplingReservoir, reset) {
    Metrics::SamplingReservoir<> dut{3};

    dut.update(-1);
    dut.reset();
    EXPECT_EQ(0, dut.samples());
    dut.update(2);
    EXPECT_EQ(1, dut.samples());
    auto snapshot = dut.getSnapshot();
    EXPECT_EQ(1, snapshot.size());
    EXPECT_EQ(2, snapshot.values()[0]);
}

TEST(TestSamplingReservoir, tinyReservoirCorrectBehaviour) {
    // test on smallest possible reservoir to see if reservoir sampling behaves
    // correctly - all samples must be selected with nearly same probability
    constexpr int RESERVOIR_SIZE = 1;
    constexpr int RUNS_PER_UPDATE = 10000;
    constexpr double MAX_REL_DEVIATION = 0.05;
    const std::vector<int> no_updates = {2, 10, 100};

    Metrics::SamplingReservoir<int> dut{RESERVOIR_SIZE};

    for (auto updates : no_updates) {
        // Put <updates> different values in the reservoir, and register in
        // stats which values where in the snapshot. Repeat this RUNS_PER_UPDATE
        // * <updates> times. In the ideal case, each value is found
        // RUNS_PER_UPDATE times.
        int stats[updates];
        // gcc allows 'int stats[updates]{}' to initialize, clang++ not
        for (int i = 0; i < updates; i++) {
            stats[i] = 0;
        }

        const int NO_RUNS = RUNS_PER_UPDATE * updates;
        for (int i = 0; i < NO_RUNS; i++) {
            dut.reset();
            for (int j = 0; j < updates; j++) {
                dut.update(j);
            }
            auto val = dut.getSnapshot().values()[0];
            stats[val]++;
        }

        for (int i = 0; i < updates; i++) {
            EXPECT_LT(stats[i], RESERVOIR_SIZE * RUNS_PER_UPDATE *
                                    (1.0 + MAX_REL_DEVIATION));
            EXPECT_GT(stats[i], RESERVOIR_SIZE * RUNS_PER_UPDATE *
                                    (1.0 - MAX_REL_DEVIATION));
        }
    }
}

TEST(TestSamplingReservoir, smallReservoirCorrectBehaviour) {
    // test on relatively small reservoir to see if reservoir sampling behaves
    // correctly - all samples must be selected with nearly same probability
    constexpr int RESERVOIR_SIZE = 100;
    constexpr int RUNS_PER_UPDATE = 100;
    constexpr double MAX_REL_DEVIATION = 0.05;
    const std::vector<int> no_updates = {1000};

    Metrics::SamplingReservoir<int> dut{RESERVOIR_SIZE};

    for (auto updates : no_updates) {
        // Put <updates> different values in the reservoir, and register in
        // stats which values where in the snapshot. Repeat this RUNS_PER_UPDATE
        // * <updates> times. In the ideal case, each value is found
        // RUNS_PER_UPDATE * RESERVOIR_SIZE times.
        int stats[updates];
        // gcc allows 'int stats[updates]{}' to initialize, clang++ not
        for (int i = 0; i < updates; i++) {
            stats[i] = 0;
        }

        const int NO_RUNS = RUNS_PER_UPDATE * updates;
        for (int i = 0; i < NO_RUNS; i++) {
            dut.reset();
            for (int j = 0; j < updates; j++) {
                dut.update(j);
            }
            auto values = dut.getSnapshot().values();
            for (auto val : values) {
                stats[val]++;
            }
        }

        for (int i = 0; i < updates; i++) {
            EXPECT_LT(stats[i], RESERVOIR_SIZE * RUNS_PER_UPDATE *
                                    (1.0 + MAX_REL_DEVIATION));
            EXPECT_GT(stats[i], RESERVOIR_SIZE * RUNS_PER_UPDATE *
                                    (1.0 - MAX_REL_DEVIATION));
        }
    }
}
} // namespace
