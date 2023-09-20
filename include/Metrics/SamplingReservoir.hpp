#ifndef METRICS_SAMPLINGRESERVOIR_HPP
#define METRICS_SAMPLINGRESERVOIR_HPP

/* Reservoir sampling in C++
   https://en.wikipedia.org/wiki/Reservoir_sampling
*/

#include "IReservoir.hpp"
#include <mutex>
#include <random>
#include <vector>

namespace Metrics {
/** create sample reservoir on a stream of data */
template <typename T = double, typename M = std::mutex>
class SamplingReservoir : public IReservoir<T> {
  public:
    explicit SamplingReservoir(unsigned n)
        : _distribution_index(0, n - 1), _reservoir(n) {
        reinitialize();
    }

    void reset() override {
        const std::lock_guard<M> lock(_mutex);
        reinitialize();
    }

    /** Update reservoir using fast algorithm L */
    void update(T value) override {
        const std::lock_guard<M> lock(_mutex);
        auto n = static_cast<unsigned>(_reservoir.size());
        if (_count < n) {
            _reservoir[_count] = value;
        } else if (_count == _next) {
            int index = _distribution_index(_random);
            _reservoir[index] = value;
            skip();
        }
        _count++;
    }

    unsigned count() const { return _count; }
    unsigned size() const override { return _reservoir.size(); }
    unsigned samples() const override {
        const std::lock_guard<M> lock(_mutex);
        return samples_nolock();
    }
    const T *data() const override { return _reservoir.data(); }

    Snapshot<T> getSnapshot() const override {
        const std::lock_guard<M> lock(_mutex);
        return Snapshot<T>(_reservoir.cbegin(),
                           _reservoir.cbegin() + samples_nolock());
    }

  private:
    /** get a random number in range ]0:1[ */
    double getRandom() {
        double r;
        do {
            r = _distribution_real(_random);
        } while (r == 0.0);
        return r;
    }

    void skip() {
        _next += std::floor(std::log(getRandom()) / std::log(1 - _w)) + 1;
        _w *= std::exp(std::log(getRandom()) / _reservoir.size());
    }

    /** non-virtual function to initialize, can be caled from constructor */
    void reinitialize() {
        _count = 0;

        auto n = static_cast<int>(_reservoir.size());
        _next = n - 1;
        _w = std::exp(std::log(getRandom()) / n);
        skip();
    }

    unsigned samples_nolock() const {
        return (count() < size()) ? count() : size();
    }

    unsigned _count{};
    unsigned _next{};
    double _w{};
    /// Fast random generator, seeded
    std::minstd_rand _random{std::random_device{}()};
    std::uniform_real_distribution<> _distribution_real{0.0, 1.0};
    std::uniform_int_distribution<> _distribution_index;
    std::vector<T> _reservoir;
    mutable M _mutex{};
};

} // namespace Metrics

#endif
