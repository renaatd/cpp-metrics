#ifndef METRICS_HISTOGRAM_HPP
#define METRICS_HISTOGRAM_HPP

#include "IMetric.hpp"
#include "IReservoir.hpp"
#include "Statistics.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

namespace Metrics {
/** Store a reservoir histogram. U is float/double, T is an IReservoir<U> */
template <typename T, typename U = double> class Histogram : public IMetric {
  public:
    /** Create a histogram
     * n = reservoir size
     * withStats = true if the output must contain stats (stdev, ...)
     * noBins > 1 if the output must contains bins */
    explicit Histogram(int n, bool withStats = false, int noBins = -1)
        : _reservoir(n), _withStats(withStats), _noBins(noBins) {}

    void reset() override { _reservoir.reset(); }
    void update(U value) { _reservoir.update(value); }
    Snapshot<U> getSnapshot() { return _reservoir.getSnapshot(); }

    std::string toString(int precision = -1) const override {
        auto snapshot = _reservoir.getSnapshot();

        std::ostringstream os;
        if (precision > -1) {
            os << std::fixed << std::setprecision(precision);
        }

        os << "count(" << snapshot.size() << "), min(" << snapshot.getValue(0)
           << "), Q25(" << snapshot.getValue(0.25) << "), Q50("
           << snapshot.getValue(0.50) << "), Q75(" << snapshot.getValue(0.75)
           << "), max(" << snapshot.getValue(1.00) << ")";

        if (_withStats) {
            Statistics<U> stats{};
            for (auto x : snapshot.values()) {
                stats.update(x);
            }
            os << ", stats: (" << stats.toString(precision) << ")";
        }
        if (_noBins > 1) {
            os << std::endl << "buckets:" << std::endl;
            dumpBinsToStream(snapshot, os);
        }
        return os.str();
    }

    void dumpBinsToStream(const Snapshot<U> &snapshot, std::ostream &os,
                          int precision = -1) const {
        // get limits of bins
        auto min = snapshot.getValue(0.0);
        auto max = snapshot.getValue(1.0);
        auto width = max - min;
        if (width <= 0.0) {
            return;
        }

        auto bins = snapshot.getBins(_noBins, min, width);
        int maxCount = *std::max_element(bins.cbegin(), bins.cend());

        // create output
        for (int i = 0; i < _noBins; i++) {
            auto count = bins[i];
            double percent = 100.0 * count / snapshot.size();
            if (precision > -1) {
                os << std::fixed << std::setprecision(precision);
            }
            os << std::setw(6) << min + i * width / _noBins
               << " <= x: " << std::setw(4) << count << " (" << std::setw(5)
               << std::setprecision(1) << std::fixed << percent << " %) - "
               << std::string(MAX_BIN_WIDTH * count / maxCount, '*')
               << std::endl;
        }
    }

  private:
    static constexpr double MAX_BIN_WIDTH =
        50; /** width of the largest bin in the output */
    T _reservoir;
    bool _withStats;
    int _noBins;
};

} // namespace Metrics

#endif
