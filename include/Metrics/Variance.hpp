#ifndef METRICS_STATISTICS_HPP
#define METRICS_STATISTICS_HPP

#include "IMetric.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

namespace Metrics {
/** Calculate 2nd order statistics incrementally using Welford's algorithm */
template <typename T = double, typename M = std::mutex>
class Variance : public IMetric {
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

        _count++;

        // use Welford's algorithm to keep errors low when there is a large
        // offset
        const T delta = value - _mean;
        _mean += delta / _count;

        const T delta2 = value - _mean;
        _m2 += delta * delta2;
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

    std::string toString(int precision = -1) const override {
        const std::lock_guard<M> lock(_mutex);
        std::ostringstream os;
        if (precision > -1) {
            os << std::fixed << std::setprecision(precision);
        }
        os << "count(" << count_nolock() << ") min(" << min_nolock()
           << ") mean(" << mean_nolock() << ") max(" << max_nolock()
           << ") stddev(" << stddev_nolock() << ") sample_stddev("
           << sample_stddev_nolock() << ")";
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

    int _count = 0;
    T _min{};
    T _max{};
    T _mean{};
    T _m2{};
    mutable M _mutex{};
};

} // namespace Metrics

#endif
