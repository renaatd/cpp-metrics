#ifndef METRICS_VARIANCE_HPP
#define METRICS_VARIANCE_HPP

#include "IMetric.hpp"
#include "MinMax.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

namespace Metrics {
namespace Internals {
/** Calculate 2nd order statistics incrementally using Welford's algorithm */
template <typename T = double> class VarianceNoLock {
  public:
    void reset() {
        _minmax.reset();
        _mean = {};
        _m2 = {};
    }

    void update(T value) {
        _minmax.update(value);

        // use Welford's algorithm to keep errors low when there is a large
        // offset
        const T delta = value - _mean;
        _mean += delta / _minmax.count();

        const T delta2 = value - _mean;
        _m2 += delta * delta2;
    }

    VarianceNoLock &operator+=(const VarianceNoLock &rhs) {
        const auto count_both = count() + rhs.count();
        if (count_both == 0) {
            return *this;
        }
        const T delta = rhs._mean - _mean;
        _mean = (count() * _mean + rhs.count() * rhs._mean) / count_both;
        _m2 += rhs._m2 + delta * delta * count() * rhs.count() / count_both;
        _minmax += rhs._minmax;
        return *this;
    }

    int64_t count() const { return _minmax.count(); }

    T min() const { return _minmax.min(); }

    T mean() const { return (_minmax.count() == 0) ? NAN : _mean; }

    T max() const { return _minmax.max(); }

    /** variance of a population */
    T variance() const {
        return (_minmax.count() < 1) ? NAN : (_m2 / _minmax.count());
    }

    /** standard deviation of a population */
    T stddev() const { return sqrt(variance()); }

    /** variance of a sample from a population */
    T sample_variance() const {
        return (_minmax.count() < 2) ? NAN : (_m2 / (_minmax.count() - 1));
    }

    /** standard deviation of a sample of a population */
    T sample_stddev() const { return sqrt(sample_variance()); }

    std::string toString(int precision = -1) const {
        std::ostringstream os;
        if (precision > -1) {
            os << std::fixed << std::setprecision(precision);
        }
        os << "count(" << _minmax.count() << ") min(" << min() << ") mean("
           << mean() << ") max(" << max() << ") sample_stddev("
           << sample_stddev() << ")";
        return os.str();
    }

  private:
    MinMaxNoLock<T> _minmax{};
    T _mean{};
    T _m2{};
};

} // namespace Internals
/** Calculate 2nd order statistics incrementally using Welford's algorithm */
template <typename T = double, typename M = std::mutex>
class Variance : public IMetric {
    using lock_guard = const std::lock_guard<M>;

  public:
    Variance() = default;
    ~Variance() override = default;

    Variance(const Variance &other) {
        // copy constructor
        lock_guard lock_other(other._mutex);
        _state = other._state;
    }

    Variance &operator=(const Variance &other) {
        // copy assignment
        if (this == &other) {
            return *this;
        }
        // In the very unlikely case that 2 threads simultaneously do a=b and
        // b=a, regular lock_guard causes a deadlock
        std::unique_lock<M> lock1{_mutex, std::defer_lock};
        std::unique_lock<M> lock2{other._mutex, std::defer_lock};
        std::lock(lock1, lock2);
        _state = other._state;
        return *this;
    }

    void reset() override {
        lock_guard lock(_mutex);
        _state.reset();
    }

    void update(T value) {
        lock_guard lock(_mutex);
        _state.update(value);
    }

    Variance &operator+=(const Variance &rhs) {
        if (&rhs == this) {
            // second lock_guard would deadlock when doing a+=a
            lock_guard lock(_mutex);
            _state += rhs._state;
            return *this;
        }

        // In the very unlikely case that 2 threads simultaneously do a+=b and
        // b+=a, regular lock_guard causes a deadlock
        std::unique_lock<M> lock1{_mutex, std::defer_lock};
        std::unique_lock<M> lock2{rhs._mutex, std::defer_lock};
        std::lock(lock1, lock2);

        _state += rhs._state;
        return *this;
    }

    friend inline Variance operator+(const Variance &lhs, const Variance &rhs) {
        Variance result = lhs;
        result += rhs;
        return result;
    }

    int64_t count() const {
        lock_guard lock(_mutex);
        return _state.count();
    }

    T min() const {
        lock_guard lock(_mutex);
        return _state.min();
    }

    T mean() const {
        lock_guard lock(_mutex);
        return _state.mean();
    }

    T max() const {
        lock_guard lock(_mutex);
        return _state.max();
    }

    /** variance of a population */
    T variance() const {
        lock_guard lock(_mutex);
        return _state.variance();
    }

    /** standard deviation of a population */
    T stddev() const {
        lock_guard lock(_mutex);
        return _state.stddev();
    }

    /** variance of a sample from a population */
    T sample_variance() const {
        lock_guard lock(_mutex);
        return _state.sample_variance();
    }

    /** standard deviation of a sample of a population */
    T sample_stddev() const {
        lock_guard lock(_mutex);
        return _state.sample_stddev();
    }

    std::string toString(int precision = -1) const override {
        lock_guard lock(_mutex);
        return _state.toString(precision);
    }

  private:
    Internals::VarianceNoLock<T> _state{};
    mutable M _mutex{};
};

} // namespace Metrics

#endif
