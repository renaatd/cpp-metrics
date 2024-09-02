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
    void reset() noexcept {
        _minmax.reset();
        _mean = {};
        _m2 = {};
    }

    void update(T value) noexcept {
        _minmax.update(value);

        // use Welford's algorithm to keep errors low when there is a large
        // offset
        const T delta = value - _mean;
        _mean += delta / _minmax.count();

        const T delta2 = value - _mean;
        _m2 += delta * delta2;
    }

    VarianceNoLock &operator+=(const VarianceNoLock &rhs) noexcept {
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

    /** return no of measurements */
    int64_t count() const noexcept { return _minmax.count(); }

    /** return lowest measured value or NAN when there are no measurements */
    T min() const noexcept { return _minmax.min(); }

    /** return mean of measured values or NAN when there are no measurements */
    T mean() const noexcept { return (_minmax.count() == 0) ? NAN : _mean; }

    /** return mean of measured values or 0 when there are no measurements */
    T mean0() const noexcept { return _mean; }

    /** return highest measured value or NAN when there are no measurements */
    T max() const noexcept { return _minmax.max(); }

    /** second order moment: sum of (x-x_mean)^2 */
    T m2() const noexcept { return _m2; }

    /** return variance of a population or NAN when there are no measurements */
    T variance() const noexcept {
        return (_minmax.count() < 1) ? NAN : (_m2 / _minmax.count());
    }

    /** standard deviation of a population */
    T stddev() const noexcept { return sqrt(variance()); }

    /** variance of a sample from a population */
    T sample_variance() const noexcept {
        return (_minmax.count() < 2) ? NAN : (_m2 / (_minmax.count() - 1));
    }

    /** standard deviation of a sample of a population */
    T sample_stddev() const noexcept { return sqrt(sample_variance()); }

    /** RMS value of the samples */
    T rms() const noexcept {
        return (_minmax.count() < 1)
                   ? NAN
                   : sqrt(_mean * _mean + _m2 / _minmax.count());
    }

    std::string toString(int precision = -1) const noexcept {
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

    Variance(const Variance &other) noexcept {
        // copy constructor
        lock_guard lock_other(other._mutex);
        _state = other._state;
    }

    Variance &operator=(const Variance &other) noexcept {
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

    void reset() noexcept override {
        lock_guard lock(_mutex);
        _state.reset();
    }

    void update(T value) noexcept {
        lock_guard lock(_mutex);
        _state.update(value);
    }

    Variance &operator+=(const Variance &rhs) noexcept {
        // In the very unlikely case that 2 threads simultaneously do a+=b and
        // b+=a, regular lock_guard causes a deadlock
        std::unique_lock<M> lock1{_mutex, std::defer_lock};
        std::unique_lock<M> lock2{rhs._mutex, std::defer_lock};
        if (&rhs == this) {
            // second lock would deadlock when doing a+=a
            lock1.lock();
        } else {
            std::lock(lock1, lock2);
        }
        _state += rhs._state;
        return *this;
    }

    friend inline Variance operator+(const Variance &lhs,
                                     const Variance &rhs) noexcept {
        Variance result = lhs;
        result += rhs;
        return result;
    }

    /** return no of measurements */
    int64_t count() const noexcept {
        lock_guard lock(_mutex);
        return _state.count();
    }

    /** return lowest measured value or NAN when there are no measurements */
    T min() const noexcept {
        lock_guard lock(_mutex);
        return _state.min();
    }

    /** return mean of measured values or NAN when there are no measurements */
    T mean() const noexcept {
        lock_guard lock(_mutex);
        return _state.mean();
    }

    /** return mean of measured values or 0 when there are no measurements */
    T mean0() const noexcept {
        lock_guard lock(_mutex);
        return _state.mean0();
    }

    /** return highest measured value or NAN when there are no measurements */
    T max() const noexcept {
        lock_guard lock(_mutex);
        return _state.max();
    }

    /** second order moment: sum of (x-x_mean)^2 */
    T m2() const noexcept {
        lock_guard lock(_mutex);
        return _state.m2();
    }

    /** return variance of a population or NAN when there are no measurements */
    T variance() const noexcept {
        lock_guard lock(_mutex);
        return _state.variance();
    }

    /** standard deviation of a population */
    T stddev() const noexcept {
        lock_guard lock(_mutex);
        return _state.stddev();
    }

    /** variance of a sample from a population */
    T sample_variance() const noexcept {
        lock_guard lock(_mutex);
        return _state.sample_variance();
    }

    /** standard deviation of a sample of a population */
    T sample_stddev() const noexcept {
        lock_guard lock(_mutex);
        return _state.sample_stddev();
    }

    /** RMS value of the samples */
    T rms() const noexcept {
        lock_guard lock(_mutex);
        return _state.rms();
    }

    std::string toString(int precision = -1) const noexcept override {
        lock_guard lock(_mutex);
        return _state.toString(precision);
    }

  private:
    Internals::VarianceNoLock<T> _state{};
    mutable M _mutex{};
};

} // namespace Metrics

#endif
