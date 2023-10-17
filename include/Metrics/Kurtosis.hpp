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
    void reset() {
        _minmax.reset();
        _mean = {};
        _m2 = {};
        _m3 = {};
        _m4 = {};
    }

    void update(T value) {
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

    T excess_kurtosis() const { return kurtosis() - 3; }

    T kurtosis() const { return (_minmax.count() * _m4) / (_m2 * _m2); }

    T skew() const {
        return sqrt(static_cast<T>(_minmax.count())) * _m3 / (pow(_m2, 1.5));
    }

    std::string toString(int precision = -1) const {
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

    Kurtosis(const Kurtosis &other) {
        // copy constructor
        lock_guard lock_other(other._mutex);
        _state = other._state;
    }

    Kurtosis &operator=(const Kurtosis &other) {
        // copy assignment
        if (this == &other) {
            return *this;
        }
        lock_guard lock(_mutex);
        lock_guard lock_other(other._mutex);
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

    T excess_kurtosis() const {
        lock_guard lock(_mutex);
        return _state.excess_kurtosis();
    }

    T kurtosis() const {
        lock_guard lock(_mutex);
        return _state.kurtosis();
    }

    T skew() const {
        lock_guard lock(_mutex);
        return _state.skew();
    }

    std::string toString(int precision = -1) const override {
        lock_guard lock(_mutex);
        return _state.toString(precision);
    }

  private:
    Internals::KurtosisNoLock<T> _state{};
    mutable M _mutex{};
};

} // namespace Metrics

#endif