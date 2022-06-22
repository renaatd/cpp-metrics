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
/** Calculate statistics incrementally using Welford's algorithm */
template <typename T = double> class Statistics : public IMetric {
  public:
    void reset() override {
        _count = 0;
        _mean = 0;
        _m2 = 0;
    }

    void update(T value) {
        std::lock_guard<std::mutex> lock(_mutex);
        // use Welford's algorithm to keep errors low when there is a large
        // offset
        if (_count == 0) {
            _min = value;
            _max = value;
        } else {
            _min = std::min(value, _min);
            _max = std::max(value, _max);
        }

        _count++;

        const T delta = value - _mean;
        _mean += delta / _count;

        const T delta2 = value - _mean;
        _m2 += delta * delta2;
    }

    int count() const { return _count; }

    T min() const { return (_count == 0) ? NAN : _min; }

    T mean() const { return (_count == 0) ? NAN : _mean; }

    T max() const { return (_count == 0) ? NAN : _max; }

    /** variance of a population */
    T variance() const { return (_count < 2) ? NAN : (_m2 / _count); }

    /** standard deviation of a population */
    T stddev() const { return sqrt(variance()); }

    /** variance of a sample from a population */
    T sample_variance() const {
        return (_count < 2) ? NAN : (_m2 / (_count - 1));
    }

    /** standard deviation of a sample of a population */
    T sample_stddev() const { return sqrt(sample_variance()); }

    std::string toString(int precision = -1) const override {
        std::lock_guard<std::mutex> lock(_mutex);
        std::ostringstream os;
        if (precision > -1) {
            os << std::fixed << std::setprecision(precision);
        }
        os << "count(" << count() << ") min(" << min() << ") mean(" << mean()
           << ") max(" << max() << ") stddev(" << stddev() << ") sample_stddev("
           << sample_stddev() << ")";
        return os.str();
    }

  private:
    int _count = 0;
    T _mean = 0;
    T _m2 = 0;
    T _min = 0;
    T _max = 0;
    mutable std::mutex _mutex{};
};

} // namespace Metrics

#endif
