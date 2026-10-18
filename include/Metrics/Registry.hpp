#ifndef METRICS_REGISTRY_HPP
#define METRICS_REGISTRY_HPP

#include "IMetric.hpp"
#include <map>
#include <memory>
#include <string>

namespace Metrics {
class Registry {
  public:
    /** add a metric to the registry while sharing ownership. If the name
     * already exists, an exception is thrown. */
    void addMetric(const std::string &name, std::shared_ptr<IMetric> metric) {
        if (_registry.find(name) != _registry.end()) {
            throw std::invalid_argument(
                "Metric with the same name already exists");
        }
        _registry[name] = std::move(metric);
    }

    std::map<std::string, std::string> reportMap(int precision = -1) {
        std::map<std::string, std::string> result = {};
        for (const auto &x : _registry) {
            result[x.first] = x.second->toString(precision);
        }
        return result;
    }

    std::string reportString(int precision = -1) {
        auto map = reportMap(precision);
        std::string result;
        for (const auto &x : map) {
            result += x.first + ": " + x.second + "\n";
        }
        return result;
    }

    void resetMetrics() {
        for (const auto &x : _registry) {
            x.second->reset();
        }
    }

  private:
    std::map<std::string, std::shared_ptr<IMetric>> _registry{};
};

} // namespace Metrics

#endif
