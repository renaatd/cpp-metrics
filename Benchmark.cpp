#include "Metrics/Gauge.hpp"
#include "Metrics/Histogram.hpp"
#include "Metrics/Kurtosis.hpp"
#include "Metrics/MinMax.hpp"
#include "Metrics/MinMeanMax.hpp"
#include "Metrics/Registry.hpp"
#include "Metrics/SamplingReservoir.hpp"
#include "Metrics/SlidingWindowReservoir.hpp"
#include "Metrics/Variance.hpp"
#include "elapsed.hpp"
#include <iostream>
#include <vector>

const int LOOPS_UPDATE = 5000000;
const int LOOPS_SNAPSHOT = 10000;
const int LOOPS_OUTPUT = 10000;

class DummyMutex {
  public:
    void lock() {}
    void unlock() {}
};

int main() {
    std::cout << "Looping, no of iterations: " << LOOPS_UPDATE << std::endl;
    {
        std::cout << "MinMax<>()" << std::endl;
        Metrics::MinMax<> stats;
        Elapsed s;
        for (int i = 0; i < LOOPS_UPDATE; i++) {
            stats.update(i);
        }
        double ns_per_loop =
            static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
        std::cout << "Stats: " << stats.toString(1) << std::endl;
        printf("time per loop: %.1lf ns\n\n", ns_per_loop);
    }

    {
        std::cout << "MinMeanMax<>()" << std::endl;
        Metrics::MinMeanMax<> stats;
        Elapsed s;
        for (int i = 0; i < LOOPS_UPDATE; i++) {
            stats.update(i);
        }
        double ns_per_loop =
            static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
        std::cout << "Stats: " << stats.toString(1) << std::endl;
        printf("time per loop: %.1lf ns\n\n", ns_per_loop);
    }

    {
        std::cout << "Variance<>()" << std::endl;
        Metrics::Variance<> stats;
        Elapsed s;
        for (int i = 0; i < LOOPS_UPDATE; i++) {
            stats.update(i);
        }
        double ns_per_loop =
            static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
        std::cout << "Stats: " << stats.toString(1) << std::endl;
        printf("time per loop: %.1lf ns\n\n", ns_per_loop);
    }

    {
        std::cout << "Variance<double,DummyMutex>()" << std::endl;
        Metrics::Variance<double, DummyMutex> stats;
        Elapsed s;
        for (int i = 0; i < LOOPS_UPDATE; i++) {
            stats.update(i);
        }
        double ns_per_loop =
            static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
        std::cout << "Stats: " << stats.toString(1) << std::endl;
        printf("time per loop: %.1lf ns\n\n", ns_per_loop);
    }

    {
        std::cout << "Kurtosis<double,DummyMutex>()" << std::endl;
        Metrics::Kurtosis<double, DummyMutex> stats;
        Elapsed s;
        for (int i = 0; i < LOOPS_UPDATE; i++) {
            stats.update(i);
        }
        double ns_per_loop =
            static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
        std::cout << "Stats: " << stats.toString(1) << std::endl;
        printf("time per loop: %.1lf ns\n\n", ns_per_loop);
    }

    {
        std::cout << "SamplingReservoir<double>(10)" << std::endl;
        Metrics::SamplingReservoir<double> reservoir(10);
        Elapsed s;
        for (int i = 0; i < LOOPS_UPDATE; i++) {
            reservoir.update(i);
        }
        double ns_per_loop =
            static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
        printf("time per loop: %.1lf ns\n\n", ns_per_loop);
    }

    {
        std::cout << "SamplingReservoir<double>(10000)" << std::endl;
        Metrics::SamplingReservoir<double> reservoir(10000);
        Elapsed s;
        for (int i = 0; i < LOOPS_UPDATE; i++) {
            reservoir.update(i);
        }
        double ns_per_loop =
            static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
        printf("time per loop: %.1lf ns\n\n", ns_per_loop);
    }

    {
        std::cout << "SamplingReservoir<double,DummyMutex>(10000)" << std::endl;
        Metrics::SamplingReservoir<double, DummyMutex> reservoir(10000);
        Elapsed s;
        for (int i = 0; i < LOOPS_UPDATE; i++) {
            reservoir.update(i);
        }
        double ns_per_loop =
            static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
        printf("time per loop: %.1lf ns\n\n", ns_per_loop);
    }

    {
        std::cout << "SlidingWindowReservoir<double>(10000)" << std::endl;
        Metrics::SlidingWindowReservoir<double> reservoir(10000);
        Elapsed s;
        for (int i = 0; i < LOOPS_UPDATE; i++) {
            reservoir.update(i);
        }
        double ns_per_loop =
            static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
        printf("time per loop: %.1lf ns\n\n", ns_per_loop);
    }

    {
        std::cout << "Gauge<double>()" << std::endl;
        Metrics::Gauge<double> gauge{};
        Elapsed s;
        for (int i = 0; i < LOOPS_UPDATE; i++) {
            gauge.update(i);
        }
        double ns_per_loop =
            static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
        printf("time per loop: %.1lf ns\n\n", ns_per_loop);
    }

    {
        std::cout << "Histogram<SamplingReservoir<double>>(1000)" << std::endl;
        Metrics::Histogram<Metrics::SamplingReservoir<double>, double>
            histogram{1000, true, 21};
        Elapsed elapsedUpdate;
        for (int i = 0; i < LOOPS_UPDATE; i++) {
            histogram.update(i);
        }
        double ns_per_loop = static_cast<double>(elapsedUpdate.ElapsedUs()) *
                             1000.0 / LOOPS_UPDATE;
        printf("time per loop: %.1lf ns\n", ns_per_loop);

        Elapsed elapsedSnapshot;
        for (int i = 0; i < LOOPS_SNAPSHOT; i++) {
            auto snapshot = histogram.getSnapshot();
        }
        double snapshot_us_per_loop =
            static_cast<double>(elapsedSnapshot.ElapsedUs()) / LOOPS_SNAPSHOT;
        printf("snapshot time per loop: %.1lf us\n", snapshot_us_per_loop);

        Elapsed elapsedOutput;
        for (int i = 0; i < LOOPS_OUTPUT; i++) {
            auto output = histogram.toString();
        }
        double output_us_per_loop =
            static_cast<double>(elapsedOutput.ElapsedUs()) / LOOPS_OUTPUT;
        printf("output time per loop: %.1lf us\n", output_us_per_loop);

        std::cout << histogram.toString(1) << std::endl;
    }

    {
        std::cout << "Histogram<SlidingWindowReservoir<double>,double>(10000)"
                  << std::endl;
        Metrics::Histogram<Metrics::SlidingWindowReservoir<double>, double>
            histogram{10000, true, 21};
        Elapsed elapsedUpdate;
        for (int i = 0; i < LOOPS_UPDATE; i++) {
            histogram.update(i);
        }
        double ns_per_loop = static_cast<double>(elapsedUpdate.ElapsedUs()) *
                             1000.0 / LOOPS_UPDATE;
        printf("time per loop: %.1lf ns\n", ns_per_loop);

        Elapsed elapsedSnapshot;
        for (int i = 0; i < LOOPS_SNAPSHOT; i++) {
            auto snapshot = histogram.getSnapshot();
        }
        double snapshot_us_per_loop =
            static_cast<double>(elapsedSnapshot.ElapsedUs()) / LOOPS_SNAPSHOT;
        printf("snapshot time per loop: %.1lf us\n", snapshot_us_per_loop);

        Elapsed elapsedOutput;
        for (int i = 0; i < LOOPS_OUTPUT; i++) {
            auto output = histogram.toString();
        }
        double output_us_per_loop =
            static_cast<double>(elapsedOutput.ElapsedUs()) / LOOPS_OUTPUT;
        printf("output time per loop: %.1lf us\n", output_us_per_loop);

        std::cout << histogram.toString(1) << std::endl << std::endl;
    }

    {
        std::cout << "Histogram<SamplingReservoir<double>,double>(1000)"
                  << std::endl;
        Metrics::Histogram<Metrics::SamplingReservoir<double>, double>
            histogram{1000, true, 31};
        Metrics::Variance<double> stats{};
        std::mt19937 random_generator;
        std::normal_distribution<> distribution{100, 10};

        // first generate a vector with all random values
        Elapsed elapsedGenerate;
        auto values = std::vector<double>(10000, 0.0);
        for (unsigned i = 0; i < values.size(); i++) {
            values[i] = distribution(random_generator);
        }
        double generate_ns_per_value =
            static_cast<double>(elapsedGenerate.ElapsedUs()) * 1000.0 /
            values.size();
        printf("generate random values, time per value: %.1lf ns\n",
               generate_ns_per_value);

        for (unsigned i = 0; i < 100; i++) {
            histogram.update(values[i]);
            stats.update(values[i]);
        }
        std::cout << "After 100 adds:" << std::endl << histogram.toString(1);
        std::cout << "Stats: " << stats.toString(1) << std::endl << std::endl;

        for (unsigned i = 100; i < values.size(); i++) {
            histogram.update(values[i]);
            stats.update(values[i]);
        }
        std::cout << "After " << values.size() << " adds:" << std::endl
                  << histogram.toString(1);
        std::cout << "Stats: " << stats.toString(1) << std::endl << std::endl;
    }

    {
        Metrics::Registry registry;
        auto gauge = std::make_shared<Metrics::Gauge<double>>();
        auto stats = std::make_shared<Metrics::Variance<double>>();

        registry.addMetric("my gauge", gauge);
        registry.addMetric("my stats", stats);

        {
            Elapsed s;
            for (int i = 0; i < LOOPS_UPDATE; i++) {
                gauge->update(i);
            }
            double ns_per_loop =
                static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
            printf("updating shared_ptr<Gauge> time per loop: %.1lf ns\n",
                   ns_per_loop);
        }
        {
            Elapsed s;
            for (int i = 0; i < LOOPS_UPDATE; i++) {
                stats->update(i);
            }
            double ns_per_loop =
                static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
            printf("updating shared_ptr<Variance> time per loop: %.1lf ns\n",
                   ns_per_loop);
        }
        std::cout << "Registry:" << std::endl
                  << registry.reportString(1) << std::endl;
    }

    return 0;
}
