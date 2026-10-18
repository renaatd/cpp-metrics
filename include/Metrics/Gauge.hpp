#ifndef METRICS_GAUGE_HPP
#define METRICS_GAUGE_HPP

#include "IMetric.hpp"
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

namespace Metrics {
/** Store a single value (gauge) */
template <typename T = double, typename M = std::mutex>
class Gauge : public IMetric {
    using lock_guard = const std::lock_guard<M>;

  public:
    void reset() noexcept override {
        lock_guard lock(_mutex);
        _value = {};
    }

    void update(T value) noexcept {
        lock_guard lock(_mutex);
        _value = value;
    }

    T value() const noexcept {
        lock_guard lock(_mutex);
        return _value;
    }

    std::string toString(int precision = -1) const override {
        std::ostringstream os;
        if (precision > -1) {
            os << std::fixed << std::setprecision(precision);
        }
        os << value();
        return os.str();
    }

  private:
    T _value{};
    mutable M _mutex{};
};

} // namespace Metrics

#endif
