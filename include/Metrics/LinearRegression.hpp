#ifndef METRICS_LINEARREGRESSION_HPP
#define METRICS_LINEARREGRESSION_HPP

// https://en.wikipedia.org/wiki/Simple_linear_regression

#include "IMetric.hpp"
#include "Variance.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

namespace Metrics {
namespace Internals {
/** Calculate linear regression coefficients using least squares incrementally
 */
template <typename T = double> class LinearRegressionNoLock {
  public:
    void reset() {
        _stats_x = {};
        _stats_y = {};
        _s_xy = {};
    }

    void update(T x, T y) {
        // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Covariance
        T dx = x - _stats_x.mean();
        _stats_x.update(x);
        _stats_y.update(y);
        T dy = y - _stats_y.mean();

        if (_stats_x.count() > 1) {
            // at the first call mean() is NAN and _s_xy is 0 for the first two
            // points anyway
            _s_xy += dx * dy;
        }
    }

    LinearRegressionNoLock &operator+=(const LinearRegressionNoLock &rhs) {
        const auto count_both = count() + rhs.count();
        if (count_both == 0) {
            return *this;
        }

        // stas.mean() is NAN when there is no data -> special case for these
        if (count() == 0) {
            _s_xy = rhs._s_xy;
        } else if (rhs.count() != 0) {
            _s_xy += rhs._s_xy + (_stats_x.mean() - rhs._stats_x.mean()) *
                                     (_stats_y.mean() - rhs._stats_y.mean()) *
                                     count() * rhs.count() / count_both;
        }

        _stats_x += rhs._stats_x;
        _stats_y += rhs._stats_y;
        return *this;
    }

    int64_t count() const { return _stats_x.count(); }

    const VarianceNoLock<T> &stats_x() const { return _stats_x; }
    const VarianceNoLock<T> &stats_y() const { return _stats_y; }

    /** slope of least squares best fit, y = slope() * x + intercept(), or NAN
     * when less than 2 measurements */
    double slope() const {
        if (_stats_x.count() < 2)
            return NAN;

        T s_xx = _stats_x.m2();
        return _s_xy / s_xx;
    }

    /** intercept of least squares best fit, y = slope() * x + intercept(), or
     * NAN when less than 2 measurements */
    double intercept() const {
        return (_stats_x.count() < 2)
                   ? NAN
                   : (_stats_y.mean() - slope() * _stats_x.mean());
    }

    /** slope of least squares best fit trough (x,y), or NAN when less than 2
     * measurements */
    double slope_through(T x, T y) const {
        if (_stats_x.count() < 1)
            return NAN;
        T s_xx = _stats_x.m2();
        T x_shift = _stats_x.mean() - x;
        T y_shift = _stats_y.mean() - y;
        return (_s_xy + x_shift * y_shift) / (s_xx + x_shift * x_shift);
    }

    /** slope of least squares best fit trough origin, or NAN when less than 2
     * measurements */
    double slope_through_origin() const { return slope_through(0.0, 0.0); }

    std::string toString(int precision = -1) const {
        std::ostringstream os;
        if (precision > -1) {
            os << std::fixed << std::setprecision(precision);
        }
        os << "count(" << _stats_x.count() << ") slope(" << slope()
           << ") intercept(" << intercept() << ")";
        return os.str();
    }

  private:
    VarianceNoLock<T> _stats_x{};
    VarianceNoLock<T> _stats_y{};
    T _s_xy{};
};

} // namespace Internals

/** Calculate linear regression coefficients using least squares incrementally
 */
template <typename T = double, typename M = std::mutex>
class LinearRegression : public IMetric {
    using lock_guard = const std::lock_guard<M>;

  public:
    LinearRegression() = default;
    ~LinearRegression() override = default;

    LinearRegression(const LinearRegression &other) {
        // copy constructor
        lock_guard lock_other(other._mutex);
        _state = other._state;
    }

    LinearRegression &operator=(const LinearRegression &other) {
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

    void update(T x, T y) {
        lock_guard lock(_mutex);
        _state.update(x, y);
    }

    LinearRegression &operator+=(const LinearRegression &rhs) {
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

    friend inline LinearRegression operator+(const LinearRegression &lhs,
                                             const LinearRegression &rhs) {
        LinearRegression result = lhs;
        result += rhs;
        return result;
    }

    int64_t count() const {
        lock_guard lock(_mutex);
        return _state.count();
    }

    const Internals::VarianceNoLock<T> &stats_x() const {
        lock_guard lock(_mutex);
        return _state.stats_x();
    }

    const Internals::VarianceNoLock<T> &stats_y() const {
        lock_guard lock(_mutex);
        return _state.stats_y();
    }

    // slope of least squares best fit, y = slope() * x + intercept(), or NAN
    // when less than 2 measurements
    double slope() const {
        lock_guard lock(_mutex);
        return _state.slope();
    }

    // intercept of least squares best fit, y = slope() * x + intercept(), or
    // NAN when less than 2 measurements
    double intercept() const {
        lock_guard lock(_mutex);
        return _state.intercept();
    }

    // slope of least squares best fit trough (x,y), or NAN when no measurements
    double slope_through(T x, T y) const {
        lock_guard lock(_mutex);
        return _state.slope_through(x, y);
    }

    // slope of least squares best fit trough origin, or NAN when no
    // measurements
    double slope_through_origin() const {
        lock_guard lock(_mutex);
        return _state.slope_through_origin();
    }

    std::string toString(int precision = -1) const override {
        lock_guard lock(_mutex);
        return _state.toString(precision);
    }

  private:
    Internals::LinearRegressionNoLock<T> _state{};
    mutable M _mutex{};
};

} // namespace Metrics

#endif
