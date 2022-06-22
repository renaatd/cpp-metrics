#ifndef METRICS_SNAPSHOT_HPP
#define METRICS_SNAPSHOT_HPP

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace Metrics {
/** Sorted data samples with quantiles */
template <typename T = double> class Snapshot {
  public:
    using Bins = std::vector<uint32_t>;

    Snapshot(typename std::vector<T>::const_iterator begin,
             typename std::vector<T>::const_iterator end)
        : _snapshot(begin, end) {
        std::sort(_snapshot.begin(), _snapshot.end());
    }

    int size() const { return _snapshot.size(); }
    const std::vector<T> &values() const { return _snapshot; }

    T getValue(double quantile) const {
        if (quantile < 0.0 || quantile > 1.0) {
            throw std::invalid_argument("quantile is not in [0..1]");
        }

        if (_snapshot.empty()) {
            return {};
        }

        const size_t maxIndex = _snapshot.size() - 1;
        const double pos = quantile * maxIndex;

        if (pos < 0) {
            return _snapshot.front();
        }

        if (pos >= maxIndex) {
            return _snapshot.back();
        }

        const size_t pos_lower = std::floor(pos);
        T lower = _snapshot[pos_lower];
        T upper = _snapshot[pos_lower + 1];
        return lower + (pos - pos_lower) * (upper - lower);
    }

    /** get vector with no of elements per bin */
    Bins getBins(int noBins, T min, T width) const {
        Bins bins(noBins);
        for (auto x : _snapshot) {
            int binIndex = std::floor(noBins * (x - min) / width);
            if (binIndex < 0) {
                binIndex = 0;
            }
            if (binIndex >= noBins) {
                binIndex = noBins - 1;
            }
            bins[binIndex]++;
        }
        return bins;
    }

  private:
    std::vector<T> _snapshot;
};

} // namespace Metrics
#endif
