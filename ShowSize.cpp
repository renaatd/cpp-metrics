#include "Metrics/Gauge.hpp"
#include "Metrics/Histogram.hpp"
#include "Metrics/Kurtosis.hpp"
#include "Metrics/MinMax.hpp"
#include "Metrics/MinMeanMax.hpp"
#include "Metrics/Registry.hpp"
#include "Metrics/SamplingReservoir.hpp"
#include "Metrics/SlidingWindowReservoir.hpp"
#include "Metrics/Variance.hpp"
#include <iostream>
#include <vector>

class DummyMutex {
  public:
    void lock() {}
    void unlock() {}
};

int main() {
    {
        Metrics::MinMax<> dut;
        std::cout << "sizeof MinMax: " << sizeof dut << std::endl;
    }
    {
        Metrics::MinMeanMax<> dut;
        std::cout << "sizeof MinMeanMax: " << sizeof dut << std::endl;
    }
    {
        Metrics::Variance<> dut;
        std::cout << "sizeof Variance: " << sizeof dut << std::endl;
    }
    {
        Metrics::Variance<double, DummyMutex> dut;
        std::cout << "sizeof Variance<double,DummyMutex>: " << sizeof dut
                  << std::endl;
    }
    {
        Metrics::Kurtosis<double, DummyMutex> dut;
        std::cout << "sizeof Kurtosis<double,DummyMutex>: " << sizeof dut
                  << std::endl;
    }
    {
        Metrics::SamplingReservoir<double> dut(10);
        std::cout << "sizeof SamplingReservoir<double>(10): " << sizeof dut
                  << std::endl;
    }
    {
        Metrics::SamplingReservoir<double> dut(10000);
        std::cout << "sizeof SamplingReservoir<double>(10000): " << sizeof dut
                  << std::endl;
    }
    {
        Metrics::SlidingWindowReservoir<double> dut(10000);
        std::cout << "sizeof SlidingWindowReservoir<double>(10000): "
                  << sizeof dut << std::endl;
    }
    {
        Metrics::Histogram<Metrics::SamplingReservoir<double>, double> dut{
            1000, true, 21};
        std::cout << "sizeof Histogram<SamplingReservoir<double>>(1000): "
                  << sizeof dut << std::endl;
    }

    return 0;
}
