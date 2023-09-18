#ifndef METRICS_MINMMEANMAX_HPP
#define METRICS_MINMMEANMAX_HPP

#include "IMetric.hpp"
#include <cmath>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

namespace Metrics {
template <typename T = double> class MinMeanMax : public IMetric {
  public:
    void reset() override {
        std::lock_guard<std::mutex> lock(_mutex);
        _count = {};
        _sum = {};
    }

    void update(T value) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_count == 0) {
            _min = value;
            _max = value;
        } else {
            _min = std::min(value, _min);
            _max = std::max(value, _max);
        }

        _count++;
        _sum += value;
    }

    int count() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return count_nolock();
    }

    T min() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return min_nolock();
    }

    T mean() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return mean_nolock();
    }

    T max() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return max_nolock();
    }

    std::string toString(int precision = -1) const override {
        std::lock_guard<std::mutex> lock(_mutex);
        std::ostringstream os;
        if (precision > -1) {
            os << std::fixed << std::setprecision(precision);
        }
        os << "count(" << count_nolock() << ") min(" << min_nolock()
           << ") mean(" << mean_nolock() << ") max(" << max_nolock() << ")";
        return os.str();
    }

  private:
    int count_nolock() const { return _count; }

    T min_nolock() const { return (_count == 0) ? NAN : _min; }
    T mean_nolock() const { return (_count == 0) ? NAN : _sum / _count; }
    T max_nolock() const { return (_count == 0) ? NAN : _max; }

    mutable std::mutex _mutex{};
    int _count = 0;
    T _min{};
    T _max{};
    T _sum{};
};

} // namespace Metrics

#endif
