#ifndef METRICS_KURTOSIS_HPP
#define METRICS_KURTOSIS_HPP

#include "IMetric.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

namespace Metrics {
/** Calculate 4th order statistics online */
template <typename T = double, typename M = std::mutex>
class Kurtosis : public IMetric {
  public:
    void reset() override {
        const std::lock_guard<M> lock(_mutex);
        _count = 0;
        _mean = {};
        _m2 = {};
    }

    void update(T value) {
        const std::lock_guard<M> lock(_mutex);

        if (_count == 0) {
            _min = value;
            _max = value;
        } else {
            _min = std::min(value, _min);
            _max = std::max(value, _max);
        }

        auto n1 = _count;
        _count++;
        const T n = static_cast<T>(_count);
        const T delta = value - _mean;
        const T delta_n = delta / _count;
        const T delta_n2 = delta_n * delta_n;
        const T term1 = delta * delta_n * n1;

        _mean += delta_n;
        _m4 += term1 * delta_n2 * (n * n - 3 * n + 3) + 6 * delta_n2 * _m2 -
               4 * delta_n * _m3;
        _m3 += term1 * delta_n * (n - 2) - 3 * delta_n * _m2;
        _m2 += term1;
    }

    int count() const {
        const std::lock_guard<M> lock(_mutex);
        return count_nolock();
    }

    T min() const {
        const std::lock_guard<M> lock(_mutex);
        return min_nolock();
    }

    T mean() const {
        const std::lock_guard<M> lock(_mutex);
        return mean_nolock();
    }

    T max() const {
        const std::lock_guard<M> lock(_mutex);
        return max_nolock();
    }

    /** variance of a population */
    T variance() const {
        const std::lock_guard<M> lock(_mutex);
        return variance_nolock();
    }

    /** standard deviation of a population */
    T stddev() const {
        const std::lock_guard<M> lock(_mutex);
        return stddev_nolock();
    }

    /** variance of a sample from a population */
    T sample_variance() const {
        const std::lock_guard<M> lock(_mutex);
        return sample_variance_nolock();
    }

    /** standard deviation of a sample of a population */
    T sample_stddev() const {
        const std::lock_guard<M> lock(_mutex);
        return sample_stddev_nolock();
    }

    T excess_kurtosis() const {
        const std::lock_guard<M> lock(_mutex);
        return excess_kurtosis_nolock();
    }

    T kurtosis() const {
        const std::lock_guard<M> lock(_mutex);
        return kurtosis_nolock();
    }

    T skew() const {
        const std::lock_guard<M> lock(_mutex);
        return skew_nolock();
    }

    std::string toString(int precision = -1) const override {
        const std::lock_guard<M> lock(_mutex);
        std::ostringstream os;
        if (precision > -1) {
            os << std::fixed << std::setprecision(precision);
        }
        os << "count(" << count_nolock() << ") min(" << min_nolock()
           << ") mean(" << mean_nolock() << ") max(" << max_nolock()
           << ") stddev(" << stddev_nolock() << ") sample_stddev("
           << sample_stddev_nolock() << ") skew(" << skew_nolock()
           << ") excess_kurtosis(" << excess_kurtosis_nolock() << ")";
        return os.str();
    }

  private:
    int count_nolock() const { return _count; }

    T min_nolock() const { return (_count == 0) ? NAN : _min; }
    T mean_nolock() const { return (_count == 0) ? NAN : _mean; }
    T max_nolock() const { return (_count == 0) ? NAN : _max; }
    T variance_nolock() const { return (_count < 1) ? NAN : (_m2 / _count); }
    T stddev_nolock() const { return sqrt(variance_nolock()); }
    T sample_variance_nolock() const {
        return (_count < 2) ? NAN : (_m2 / (_count - 1));
    }
    T sample_stddev_nolock() const { return sqrt(sample_variance_nolock()); }
    T kurtosis_nolock() const { return (_count * _m4) / (_m2 * _m2); }
    T excess_kurtosis_nolock() const { return kurtosis_nolock() - 3; }
    T skew_nolock() const {
        return sqrt(static_cast<T>(_count)) * _m3 / (pow(_m2, 1.5));
    }

    int _count = 0;
    T _min{};
    T _max{};
    T _mean{};
    T _m2{};
    T _m3{};
    T _m4{};
    mutable M _mutex{};
};

} // namespace Metrics

#endif
