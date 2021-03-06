#include "Metrics/Gauge.hpp"
#include "Metrics/Histogram.hpp"
#include "Metrics/MinMeanMax.hpp"
#include "Metrics/Registry.hpp"
#include "Metrics/SamplingReservoir.hpp"
#include "Metrics/SlidingWindowReservoir.hpp"
#include "Metrics/Statistics.hpp"
#include "elapsed.hpp"
#include <iostream>

const int LOOPS_UPDATE = 5000000;
const int LOOPS_SNAPSHOT = 10000;
const int LOOPS_OUTPUT = 10000;

int main() {
  std::cout << "Looping, no of iterations: " << LOOPS_UPDATE << std::endl;
  {
    std::cout << "MinMeanMax<>()" << std::endl;
    Metrics::MinMeanMax<> stats;
    Elapsed s;
    for (int i = 0; i < LOOPS_UPDATE; i++) {
      stats.update(i);
    }
    double ns_per_loop = static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
    printf("time per loop: %.1lf ns\n\n", ns_per_loop);
  }

  {
    std::cout << "Statistics<>()" << std::endl;
    Metrics::Statistics<> stats;
    Elapsed s;
    for (int i = 0; i < LOOPS_UPDATE; i++) {
      stats.update(i);
    }
    double ns_per_loop = static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
    printf("time per loop: %.1lf ns\n\n", ns_per_loop);
  }

  {
    std::cout << "SamplingReservoir<double>(10)" << std::endl;
    Metrics::SamplingReservoir<double> reservoir(10);
    Elapsed s;
    for (int i = 0; i < LOOPS_UPDATE; i++) {
      reservoir.update(i);
    }
    double ns_per_loop = static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
    printf("time per loop: %.1lf ns\n\n", ns_per_loop);
  }

  {
    std::cout << "SamplingReservoir<double>(10000)" << std::endl;
    Metrics::SamplingReservoir<double> reservoir(10000);
    Elapsed s;
    for (int i = 0; i < LOOPS_UPDATE; i++) {
      reservoir.update(i);
    }
    double ns_per_loop = static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
    printf("time per loop: %.1lf ns\n\n", ns_per_loop);
  }

  {
    std::cout << "SlidingWindowReservoir<double>(10000)" << std::endl;
    Metrics::SlidingWindowReservoir<double> reservoir(10000);
    Elapsed s;
    for (int i = 0; i < LOOPS_UPDATE; i++) {
      reservoir.update(i);
    }
    double ns_per_loop = static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
    printf("time per loop: %.1lf ns\n\n", ns_per_loop);
  }

  {
    std::cout << "Gauge<double>()" << std::endl;
    Metrics::Gauge<double> gauge{};
    Elapsed s;
    for (int i = 0; i < LOOPS_UPDATE; i++) {
      gauge.update(i);
    }
    double ns_per_loop = static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
    printf("time per loop: %.1lf ns\n\n", ns_per_loop);
  }

  {
    std::cout << "Histogram<SamplingReservoir<double> >(1000)" << std::endl;
    Metrics::Histogram<Metrics::SamplingReservoir<double>, double> histogram{ 1000, true, 21};
    Elapsed elapsedUpdate;
    for (int i = 0; i < LOOPS_UPDATE; i++) {
      histogram.update(i);
    }
    double ns_per_loop = static_cast<double>(elapsedUpdate.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
    printf("time per loop: %.1lf ns\n", ns_per_loop);

    Elapsed elapsedSnapshot;
    for (int i = 0; i < LOOPS_SNAPSHOT; i++) {
      auto snapshot = histogram.getSnapshot();
    }
    double snapshot_us_per_loop = static_cast<double>(elapsedSnapshot.ElapsedUs()) / LOOPS_SNAPSHOT;
    printf("snapshot time per loop: %.1lf us\n", snapshot_us_per_loop);

    Elapsed elapsedOutput;
    for (int i = 0; i < LOOPS_OUTPUT; i++) {
      auto output = histogram.toString();
    }
    double output_us_per_loop = static_cast<double>(elapsedOutput.ElapsedUs()) / LOOPS_OUTPUT;
    printf("output time per loop: %.1lf us\n", output_us_per_loop);

    std::cout << histogram.toString(1) << std::endl;
  }

  {
    std::cout << "Histogram<SlidingWindowReservoir<double> >(10000)" << std::endl;
    Metrics::Histogram<Metrics::SlidingWindowReservoir<double>, double> histogram{ 10000, true, 21};
    Elapsed elapsedUpdate;
    for (int i = 0; i < LOOPS_UPDATE; i++) {
      histogram.update(i);
    }
    double ns_per_loop = static_cast<double>(elapsedUpdate.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
    printf("time per loop: %.1lf ns\n", ns_per_loop);

    Elapsed elapsedSnapshot;
    for (int i = 0; i < LOOPS_SNAPSHOT; i++) {
      auto snapshot = histogram.getSnapshot();
    }
    double snapshot_us_per_loop = static_cast<double>(elapsedSnapshot.ElapsedUs()) / LOOPS_SNAPSHOT;
    printf("snapshot time per loop: %.1lf us\n", snapshot_us_per_loop);

    Elapsed elapsedOutput;
    for (int i = 0; i < LOOPS_OUTPUT; i++) {
      auto output = histogram.toString();
    }
    double output_us_per_loop = static_cast<double>(elapsedOutput.ElapsedUs()) / LOOPS_OUTPUT;
    printf("output time per loop: %.1lf us\n", output_us_per_loop);

    std::cout << histogram.toString(1) << std::endl << std::endl;
  }

  {
    std::cout << "Histogram<SamplingReservoir<double> >(1000)" << std::endl;
    Metrics::Histogram<Metrics::SamplingReservoir<double>, double> histogram{ 1000, true, 31};
    Metrics::Statistics<double> stats { };
    std::mt19937 random_generator;
    std::normal_distribution<> distribution{100,10};
    for (int i=0; i < 100; i++) {
        auto value = distribution(random_generator);
        histogram.update(value);
        stats.update(value);
    }
    std::cout << "After 100 adds:" << std::endl << histogram.toString(1);
    std::cout << "Stats: " << stats.toString(1) << std::endl << std::endl;
 
    for (int i=0; i < 10000; i++) {
        auto value = distribution(random_generator);
        histogram.update(value);
        stats.update(value);
    }
    std::cout << "After 10000 adds:" << std::endl << histogram.toString(1);
    std::cout << "Stats: " << stats.toString(1) << std::endl << std::endl;
  }

  {
    Metrics::Registry registry;
    auto gauge = std::make_shared<Metrics::Gauge<double>>();
    auto stats = std::make_shared<Metrics::Statistics<double>>();

    registry.addMetric("my gauge", gauge);
    registry.addMetric("my stats", stats);

    {
      Elapsed s;
      for (int i = 0; i < LOOPS_UPDATE; i++) {
        gauge->update(i);
      }
      double ns_per_loop = static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
      printf("updating shared_ptr<Gauge> time per loop: %.1lf ns\n",
             ns_per_loop);
    }
    {
      Elapsed s;
      for (int i = 0; i < LOOPS_UPDATE; i++) {
        stats->update(i);
      }
      double ns_per_loop = static_cast<double>(s.ElapsedUs()) * 1000.0 / LOOPS_UPDATE;
      printf("updating shared_ptr<Statistics> time per loop: %.1lf ns\n",
             ns_per_loop);
    }
    std::cout << "Registry:" << std::endl
              << registry.reportString(1) << std::endl;
  }

  return 0;
}
