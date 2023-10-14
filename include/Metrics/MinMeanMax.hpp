#ifndef METRICS_MINMEANMAX_HPP
#define METRICS_MINMEANMAX_HPP

#include "IMetric.hpp"
#include <cmath>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

namespace Metrics {
template <typename T = double, typename M = std::mutex>
class MinMeanMax : public IMetric {
  public:
    void reset() override {
        const std::lock_guard<M> lock(_mutex);
        _count = {};
        _sum = {};
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
        _sum += value;
    }

    MinMeanMax &operator+=(const MinMeanMax &rhs) {
        const std::lock_guard<M> lock(_mutex);
        const std::lock_guard<M> lock_rhs(rhs._mutex);

        if (_count == 0 && rhs._count == 0) {
            return *this;
        }

        if (_count == 0) {
            _min = rhs._min;
            _max = rhs._max;
        } else if (rhs._count != 0) {
            _min = std::min(_min, rhs._min);
            _max = std::max(_max, rhs._max);
        }

        _count += rhs._count;
        _sum += rhs._sum;
        return *this;
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

    std::string toString(int precision = -1) const override {
        const std::lock_guard<M> lock(_mutex);
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

    int _count = 0;
    T _min{};
    T _max{};
    T _sum{};
    mutable M _mutex{};
};

} // namespace Metrics

#endif
