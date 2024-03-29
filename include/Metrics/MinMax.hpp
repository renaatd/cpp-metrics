#ifndef METRICS_MINMAX_HPP
#define METRICS_MINMAX_HPP

#include "IMetric.hpp"
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

namespace Metrics {
namespace Internals {
template <typename T = double> class MinMaxNoLock {
  public:
    void reset() noexcept { _count = {}; }

    void update(T value) noexcept {
        if (_count == 0) {
            _min = value;
            _max = value;
        } else {
            _min = std::min(value, _min);
            _max = std::max(value, _max);
        }

        _count++;
    }

    MinMaxNoLock &operator+=(const MinMaxNoLock &rhs) noexcept {
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
        return *this;
    }

    friend inline MinMaxNoLock operator+(const MinMaxNoLock &lhs,
                                         const MinMaxNoLock &rhs) noexcept {
        MinMaxNoLock result = lhs;
        result += rhs;
        return result;
    }

    /** return no of measurements */
    int64_t count() const noexcept { return _count; }

    /** return lowest measured value or NAN when there are no measurements */
    T min() const noexcept { return (_count == 0) ? NAN : _min; }

    /** return highest measured value or NAN when there are no measurements */
    T max() const noexcept { return (_count == 0) ? NAN : _max; }

    std::string toString(int precision = -1) const noexcept {
        std::ostringstream os;
        if (precision > -1) {
            os << std::fixed << std::setprecision(precision);
        }
        os << "count(" << count() << ") min(" << min() << ") max(" << max()
           << ")";
        return os.str();
    }

  private:
    int64_t _count = 0;
    T _min{};
    T _max{};
};

} // namespace Internals

template <typename T = double, typename M = std::mutex>
class MinMax : public IMetric {
    using lock_guard = const std::lock_guard<M>;

  public:
    MinMax() = default;
    ~MinMax() override = default;

    MinMax(const MinMax &other) noexcept {
        // copy constructor
        lock_guard lock_other(other._mutex);
        _state = other._state;
    }

    MinMax &operator=(const MinMax &other) noexcept {
        // copy assignment
        if (this == &other) {
            return *this;
        }
        // In the very unlikely case that 2 threads simultaneously do a=b and
        // b=a, regular lock_guard causes a deadlock
        std::unique_lock<M> lock1{_mutex, std::defer_lock};
        std::unique_lock<M> lock2{other._mutex, std::defer_lock};
        std::lock(lock1, lock2);
        _state = other._state;
        return *this;
    }

    void reset() noexcept override {
        lock_guard lock(_mutex);
        _state.reset();
    }

    void update(T value) noexcept {
        lock_guard lock(_mutex);
        _state.update(value);
    }

    MinMax &operator+=(const MinMax &rhs) noexcept {
        // In the very unlikely case that 2 threads simultaneously do a+=b and
        // b+=a, regular lock_guard causes a deadlock
        std::unique_lock<M> lock1{_mutex, std::defer_lock};
        std::unique_lock<M> lock2{rhs._mutex, std::defer_lock};
        if (&rhs == this) {
            // second lock would deadlock when doing a+=a
            lock1.lock();
        } else {
            std::lock(lock1, lock2);
        }
        _state += rhs._state;
        return *this;
    }

    friend inline MinMax operator+(const MinMax &lhs,
                                   const MinMax &rhs) noexcept {
        MinMax result = lhs;
        result += rhs;
        return result;
    }

    /** return no of measurements */
    int count() const noexcept {
        lock_guard lock(_mutex);
        return _state.count();
    }

    /** return lowest measured value or NAN when there are no measurements */
    T min() const noexcept {
        lock_guard lock(_mutex);
        return _state.min();
    }

    /** return highest measured value or NAN when there are no measurements */
    T max() const noexcept {
        lock_guard lock(_mutex);
        return _state.max();
    }

    std::string toString(int precision = -1) const noexcept override {
        lock_guard lock(_mutex);
        return _state.toString(precision);
    }

  private:
    Internals::MinMaxNoLock<T> _state{};
    mutable M _mutex{};
};

} // namespace Metrics

#endif
