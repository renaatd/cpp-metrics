#ifndef METRICS_KURTOSIS_HPP
#define METRICS_KURTOSIS_HPP

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
/** Calculate 4th order statistics online */
template <typename T = double> class KurtosisNoLock {
  public:
    void reset() noexcept {
        _minmax.reset();
        _mean = {};
        _m2 = {};
        _m3 = {};
        _m4 = {};
    }

    void update(T value) noexcept {
        auto n1 = _minmax.count();
        _minmax.update(value);

        const T n = static_cast<T>(n1 + 1);
        const T delta = value - _mean;
        const T delta_n = delta / n;
        const T delta_n2 = delta_n * delta_n;
        const T term1 = delta * delta_n * n1;

        _mean += delta_n;
        _m4 += term1 * delta_n2 * (n * n - 3 * n + 3) + 6 * delta_n2 * _m2 -
               4 * delta_n * _m3;
        _m3 += term1 * delta_n * (n - 2) - 3 * delta_n * _m2;
        _m2 += term1;
    }

    int64_t count() const noexcept { return _minmax.count(); }

    T min() const noexcept { return _minmax.min(); }

    T mean() const noexcept { return (_minmax.count() == 0) ? NAN : _mean; }

    T max() const noexcept { return _minmax.max(); }

    /** variance of a population */
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

    T excess_kurtosis() const noexcept { return kurtosis() - 3; }

    T kurtosis() const noexcept {
        return (_minmax.count() * _m4) / (_m2 * _m2);
    }

    T skew() const noexcept {
        return sqrt(static_cast<T>(_minmax.count())) * _m3 / (pow(_m2, 1.5));
    }

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
        os << "count(" << count() << ") min(" << min() << ") mean(" << mean()
           << ") max(" << max() << ") sample_stddev(" << sample_stddev()
           << ") skew(" << skew() << ") excess_kurtosis(" << excess_kurtosis()
           << ")";
        return os.str();
    }

  private:
    MinMaxNoLock<T> _minmax{};
    T _mean{};
    T _m2{};
    T _m3{};
    T _m4{};
};

} // namespace Internals
/** Calculate 4th order statistics online */
template <typename T = double, typename M = std::mutex>
class Kurtosis : public IMetric {
    using lock_guard = const std::lock_guard<M>;

  public:
    Kurtosis() = default;
    ~Kurtosis() override = default;

    Kurtosis(const Kurtosis &other) noexcept {
        // copy constructor
        lock_guard lock_other(other._mutex);
        _state = other._state;
    }

    Kurtosis &operator=(const Kurtosis &other) noexcept {
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

    int64_t count() const noexcept {
        lock_guard lock(_mutex);
        return _state.count();
    }

    T min() const noexcept {
        lock_guard lock(_mutex);
        return _state.min();
    }

    T mean() const noexcept {
        lock_guard lock(_mutex);
        return _state.mean();
    }

    T max() const noexcept {
        lock_guard lock(_mutex);
        return _state.max();
    }

    /** variance of a population */
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

    T excess_kurtosis() const noexcept {
        lock_guard lock(_mutex);
        return _state.excess_kurtosis();
    }

    T kurtosis() const noexcept {
        lock_guard lock(_mutex);
        return _state.kurtosis();
    }

    T skew() const noexcept {
        lock_guard lock(_mutex);
        return _state.skew();
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
    Internals::KurtosisNoLock<T> _state{};
    mutable M _mutex{};
};

} // namespace Metrics

#endif