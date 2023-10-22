// Tests trying to cause a deadlock (e.g. bad locking order)

#include "Metrics/MinMeanMax.hpp"
#include "Metrics/Variance.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <thread>

namespace {

constexpr int LOOPS = 1000000;

template <typename T> void assign(T &a, const T &b) {
    for (int i = 0; i < LOOPS; i++) {
        a = b;
    }
}

template <typename T> void add(T &a, const T &b) {
    for (int i = 0; i < LOOPS; i++) {
        a += b;
    }
}

TEST(TestDeadlock, minMaxAssign) {
    Metrics::MinMax<> dut1, dut2;
    std::cout << "Starting thread" << std::endl;
    std::thread t1(assign<Metrics::MinMax<>>, std::ref(dut1), std::ref(dut2));
    std::thread t2(assign<Metrics::MinMax<>>, std::ref(dut2), std::ref(dut1));

    std::cout << "Joining threads" << std::endl;
    t1.join();
    t2.join();
}

TEST(TestDeadlock, minMeanMaxAssign) {
    Metrics::MinMeanMax<> dut1, dut2;
    std::cout << "Starting thread" << std::endl;
    std::thread t1(assign<Metrics::MinMeanMax<>>, std::ref(dut1),
                   std::ref(dut2));
    std::thread t2(assign<Metrics::MinMeanMax<>>, std::ref(dut2),
                   std::ref(dut1));

    std::cout << "Joining threads" << std::endl;
    t1.join();
    t2.join();
}

TEST(TestDeadlock, varianceAssign) {
    Metrics::Variance<> dut1, dut2;
    std::cout << "Starting thread" << std::endl;
    std::thread t1(assign<Metrics::Variance<>>, std::ref(dut1), std::ref(dut2));
    std::thread t2(assign<Metrics::Variance<>>, std::ref(dut2), std::ref(dut1));

    std::cout << "Joining threads" << std::endl;
    t1.join();
    t2.join();
}

TEST(TestDeadlock, minMaxAdd) {
    Metrics::MinMax<> dut1, dut2;
    std::cout << "Starting thread" << std::endl;
    std::thread t1(add<Metrics::MinMax<>>, std::ref(dut1), std::ref(dut2));
    std::thread t2(add<Metrics::MinMax<>>, std::ref(dut2), std::ref(dut1));

    std::cout << "Joining threads" << std::endl;
    t1.join();
    t2.join();
}

TEST(TestDeadlock, minMeanMaxAdd) {
    Metrics::MinMeanMax<> dut1, dut2;
    std::cout << "Starting thread" << std::endl;
    std::thread t1(add<Metrics::MinMeanMax<>>, std::ref(dut1), std::ref(dut2));
    std::thread t2(add<Metrics::MinMeanMax<>>, std::ref(dut2), std::ref(dut1));

    std::cout << "Joining threads" << std::endl;
    t1.join();
    t2.join();
}

TEST(TestDeadlock, varianceAdd) {
    Metrics::Variance<> dut1, dut2;
    std::cout << "Starting thread" << std::endl;
    std::thread t1(add<Metrics::Variance<>>, std::ref(dut1), std::ref(dut2));
    std::thread t2(add<Metrics::Variance<>>, std::ref(dut2), std::ref(dut1));

    std::cout << "Joining threads" << std::endl;
    t1.join();
    t2.join();
}

} // namespace