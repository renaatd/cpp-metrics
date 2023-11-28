#ifndef METRICS_MINMEANMAX_HPP
#define METRICS_MINMEANMAX_HPP

#include "IMetric.hpp"
#include "MinMax.hpp"
#include <cmath>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

namespace Metrics {
namespace Internals {
template <typename T = double> class MinMeanMaxNoLock {
  public:
    void reset() {
        _minmax.reset();
        _sum = {};
    }

    void update(T value) {
        _minmax.update(value);
        _sum += value;
    }

    MinMeanMaxNoLock &operator+=(const MinMeanMaxNoLock &rhs) {
        _minmax += rhs._minmax;
        _sum += rhs._sum;
        return *this;
    }

    friend inline MinMeanMaxNoLock operator+(const MinMeanMaxNoLock &lhs,
                                             const MinMeanMaxNoLock &rhs) {
        MinMeanMaxNoLock result = lhs;
        result += rhs;
        return result;
    }

    /** return no of measurements */
    int64_t count() const { return _minmax.count(); }

    /** return lowest measured value or NAN when there are no measurements */
    T min() const { return _minmax.min(); }

    /** return mean of measured values or NAN when there are no measurements */
    T mean() const {
        auto current_count = _minmax.count();
        return (current_count == 0) ? NAN : _sum / current_count;
    }

    /** return highest measured value or NAN when there are no measurements */
    T max() const { return _minmax.max(); }

    std::string toString(int precision = -1) const {
        std::ostringstream os;
        if (precision > -1) {
            os << std::fixed << std::setprecision(precision);
        }
        os << "count(" << count() << ") min(" << min() << ") mean(" << mean()
           << ") max(" << max() << ")";
        return os.str();
    }

  private:
    MinMaxNoLock<T> _minmax{};
    T _sum{};
};
} // namespace Internals

template <typename T = double, typename M = std::mutex>
class MinMeanMax : public IMetric {
    using lock_guard = const std::lock_guard<M>;

  public:
    MinMeanMax() = default;
    ~MinMeanMax() override = default;

    MinMeanMax(const MinMeanMax &other) {
        // copy constructor
        lock_guard lock_other(other._mutex);
        _state = other._state;
    }

    MinMeanMax &operator=(const MinMeanMax &other) {
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

    void reset() override {
        lock_guard lock(_mutex);
        _state.reset();
    }

    void update(T value) {
        lock_guard lock(_mutex);
        _state.update(value);
    }

    MinMeanMax &operator+=(const MinMeanMax &rhs) {
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

    friend inline MinMeanMax operator+(const MinMeanMax &lhs,
                                       const MinMeanMax &rhs) {
        MinMeanMax result = lhs;
        result += rhs;
        return result;
    }

    /** return no of measurements */
    int64_t count() const {
        lock_guard lock(_mutex);
        return _state.count();
    }

    /** return lowest measured value or NAN when there are no measurements */
    T min() const {
        lock_guard lock(_mutex);
        return _state.min();
    }

    /** return mean of measured values or NAN when there are no measurements */
    T mean() const {
        lock_guard lock(_mutex);
        return _state.mean();
    }

    /** return highest measured value or NAN when there are no measurements */
    T max() const {
        lock_guard lock(_mutex);
        return _state.max();
    }

    std::string toString(int precision = -1) const override {
        lock_guard lock(_mutex);
        return _state.toString(precision);
    }

  private:
    Internals::MinMeanMaxNoLock<T> _state{};
    mutable M _mutex{};
};

} // namespace Metrics

#endif
