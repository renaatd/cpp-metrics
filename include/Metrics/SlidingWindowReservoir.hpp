#ifndef METRICS_SLIDINGWINDOWRESERVOIR_HPP
#define METRICS_SLIDINGWINDOWRESERVOIR_HPP

#include "IReservoir.hpp"
#include <mutex>
#include <vector>

namespace Metrics {
/** sliding windows on a stream of data */
template <typename T = double> class SlidingWindowReservoir : public IReservoir<T> {
  public:
    explicit SlidingWindowReservoir(unsigned n) : _reservoir(n) {}

    void reset() override {
        _writePosition = 0;
        _full = false;
    }

    /** Update sliding window */
    void update(T value) override {
        std::lock_guard<std::mutex> lock(_mutex);
        _reservoir[_writePosition] = value;
        _writePosition++;

        auto size = static_cast<unsigned>(_reservoir.size());
        if (_writePosition >= size) {
            _full = true;
            _writePosition = 0;
        }
    }

    unsigned size() const override { return _reservoir.size(); }
    unsigned samples() const override {
        return _full ? _reservoir.size() : _writePosition;
    }
    const T *data() const override { return _reservoir.data(); }

    Snapshot<T> getSnapshot() const override {
        std::lock_guard<std::mutex> lock(_mutex);
        return Snapshot<T>(_reservoir.cbegin(),
                           _reservoir.cbegin() + samples());
    }

  private:
    mutable std::mutex _mutex{};
    unsigned _writePosition = 0;
    bool _full = false;
    std::vector<T> _reservoir;
};

} // namespace Metrics
#endif
