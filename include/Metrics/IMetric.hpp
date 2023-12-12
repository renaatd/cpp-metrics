#ifndef METRICS_IMETRIC_HPP
#define METRICS_IMETRIC_HPP

#include <string>

namespace Metrics {
/** Interface of a metric, can be stored in a registry */
class IMetric {
  public:
    virtual void reset() noexcept = 0;
    virtual std::string toString(int precision = -1) const noexcept = 0;
    virtual ~IMetric() = default;
};

} // namespace Metrics

#endif
