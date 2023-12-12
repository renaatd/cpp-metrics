#ifndef METRICS_GAUGE_HPP
#define METRICS_GAUGE_HPP

#include "IMetric.hpp"
#include <atomic>
#include <iomanip>
#include <sstream>
#include <string>

namespace Metrics {
/** Store a single value (gauge) */
template <typename T = double> class Gauge : public IMetric {
  public:
    void reset() noexcept override { _value = {}; }

    void update(T value) noexcept {
        // _value.store(value);
        _value = value;
    }

    T value() const noexcept {
        // return _value.load();
        return _value;
    }

    std::string toString(int precision = -1) const noexcept override {
        std::ostringstream os;
        if (precision > -1) {
            os << std::fixed << std::setprecision(precision);
        }
        os << value();
        return os.str();
    }

  private:
    // std::atomic<T> _value {};
    T _value{};
};

} // namespace Metrics

#endif
