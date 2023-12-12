#ifndef METRICS_SLIDINGWINDOWRESERVOIR_HPP
#define METRICS_SLIDINGWINDOWRESERVOIR_HPP

#include "IReservoir.hpp"
#include <mutex>
#include <vector>

namespace Metrics {
/** sliding windows on a stream of data */
template <typename T = double, typename M = std::mutex>
class SlidingWindowReservoir : public IReservoir<T> {
  public:
    explicit SlidingWindowReservoir(unsigned n) : _reservoir(n) {}

    void reset() noexcept override {
        const std::lock_guard<M> lock(_mutex);
        _writePosition = 0;
        _full = false;
    }

    /** Update sliding window */
    void update(T value) noexcept override {
        const std::lock_guard<M> lock(_mutex);
        _reservoir[_writePosition] = value;
        _writePosition++;

        auto reservoir_size = static_cast<unsigned>(_reservoir.size());
        if (_writePosition >= reservoir_size) {
            _full = true;
            _writePosition = 0;
        }
    }

    unsigned size() const noexcept override { return _reservoir.size(); }
    unsigned samples() const noexcept override {
        const std::lock_guard<M> lock(_mutex);
        return samples_nolock();
    }
    const T *data() const noexcept override { return _reservoir.data(); }

    Snapshot<T> getSnapshot() const noexcept override {
        const std::lock_guard<M> lock(_mutex);
        return Snapshot<T>(_reservoir.cbegin(),
                           _reservoir.cbegin() + samples_nolock());
    }

  private:
    unsigned samples_nolock() const noexcept {
        return _full ? _reservoir.size() : _writePosition;
    }

    unsigned _writePosition = 0;
    bool _full = false;
    std::vector<T> _reservoir;
    mutable M _mutex{};
};

} // namespace Metrics
#endif
