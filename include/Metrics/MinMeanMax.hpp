#ifndef METRICS_MINMMEANMAX_HPP
#define METRICS_MINMMEANMAX_HPP

#include "IMetric.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

namespace Metrics {
template <typename T = double> class MinMeanMax : public IMetric {
  public:
    void reset() override {
        _count = {};
        _sum = {};
    }

    void update(T value) {
        if (_count == 0) {
            _min = value;
            _max = value;
        } else {
            if (value < _min)
                _min = value;
            if (value > _max)
                _max = value;
        }

        _count++;
        _sum += value;
    }

    int count() const { return _count; }

    T min() const { return (_count == 0) ? NAN : _min; }
    T mean() const { return (_count == 0) ? NAN : _sum / _count; }
    T max() const { return (_count == 0) ? NAN : _max; }

    std::string toString(int precision = -1) const override {
        std::ostringstream os;
        if (precision > -1) {
            os << std::fixed << std::setprecision(precision);
        }
        os << "count(" << count() << ") min(" << min() << ") mean(" << mean()
           << ") max(" << max() << ")";
        return os.str();
    }

  private:
    int _count = 0;
    T _sum = 0;
    T _min = 0;
    T _max = 0;
};

} // namespace Metrics

#endif
