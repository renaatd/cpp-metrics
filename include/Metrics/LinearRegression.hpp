#ifndef METRICS_LINEARREGRESSION_HPP
#define METRICS_LINEARREGRESSION_HPP

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
/** Calculate incrementally linear regression coefficients using least squares
 * https://en.wikipedia.org/wiki/Simple_linear_regression
 */
template <typename T = double> class LinearRegressionNoLock {
  public:
    void reset() noexcept {
        _stats_x = {};
        _stats_y = {};
        _s_xy = {};
    }

    void update(T x, T y) noexcept {
        // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Covariance
        T dx = x - _stats_x.mean0();
        _stats_x.update(x);
        _stats_y.update(y);
        T dy = y - _stats_y.mean0();
        _s_xy += dx * dy;
    }

    LinearRegressionNoLock &
    operator+=(const LinearRegressionNoLock &rhs) noexcept {
        const auto count_both = count() + rhs.count();
        if (count_both == 0) {
            return *this;
        }

        // Note: stats.mean() is 0 when there is no data, OK
        _s_xy += rhs._s_xy + (_stats_x.mean0() - rhs._stats_x.mean0()) *
                                 (_stats_y.mean0() - rhs._stats_y.mean0()) *
                                 count() * rhs.count() / count_both;

        _stats_x += rhs._stats_x;
        _stats_y += rhs._stats_y;
        return *this;
    }

    /** return no of measurements */
    int64_t count() const noexcept { return _stats_x.count(); }

    const VarianceNoLock<T> &stats_x() const noexcept { return _stats_x; }
    const VarianceNoLock<T> &stats_y() const noexcept { return _stats_y; }

    /** slope of least squares best fit, y = slope() * x + intercept(), or NAN
     * when less than 2 measurements */
    T slope() const noexcept {
        if (_stats_x.count() < 2) {
            return NAN;
        }

        T s_xx = _stats_x.m2();
        return _s_xy / s_xx;
    }

    /** intercept of least squares best fit, y = slope() * x + intercept(), or
     * NAN when less than 2 measurements */
    T intercept() const noexcept {
        return (_stats_x.count() < 2)
                   ? NAN
                   : (_stats_y.mean() - slope() * _stats_x.mean());
    }

    /** correlation of x and y, or NAN when less than 2 measurements */
    T correlation() const noexcept {
        return (_stats_x.count() < 2)
                   ? NAN
                   : _s_xy / (_stats_x.count() * _stats_x.stddev() *
                              _stats_y.stddev());
    }

    /** slope of least squares best fit trough (x,y), or NAN when less than 2
     * measurements */
    T slope_through(T x, T y) const noexcept {
        if (_stats_x.count() < 1) {
            return NAN;
        }

        T s_xx = _stats_x.m2();
        T x_shift = _stats_x.mean() - x;
        T y_shift = _stats_y.mean() - y;
        return (_s_xy + x_shift * y_shift) / (s_xx + x_shift * x_shift);
    }

    /** slope of least squares best fit trough origin, or NAN when less than 2
     * measurements */
    T slope_through_origin() const noexcept { return slope_through(0.0, 0.0); }

    std::string toString(int precision = -1) const noexcept {
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

/** Calculate incrementally linear regression coefficients using least squares
 * https://en.wikipedia.org/wiki/Simple_linear_regression
 */
template <typename T = double, typename M = std::mutex>
class LinearRegression : public IMetric {
    using lock_guard = const std::lock_guard<M>;

  public:
    LinearRegression() = default;
    ~LinearRegression() override = default;

    LinearRegression(const LinearRegression &other) noexcept {
        // copy constructor
        lock_guard lock_other(other._mutex);
        _state = other._state;
    }

    LinearRegression &operator=(const LinearRegression &other) noexcept {
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

    void update(T x, T y) noexcept {
        lock_guard lock(_mutex);
        _state.update(x, y);
    }

    LinearRegression &operator+=(const LinearRegression &rhs) noexcept {
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

    friend inline LinearRegression
    operator+(const LinearRegression &lhs,
              const LinearRegression &rhs) noexcept {
        LinearRegression result = lhs;
        result += rhs;
        return result;
    }

    /** return no of measurements */
    int64_t count() const noexcept {
        lock_guard lock(_mutex);
        return _state.count();
    }

    const Internals::VarianceNoLock<T> &stats_x() const noexcept {
        lock_guard lock(_mutex);
        return _state.stats_x();
    }

    const Internals::VarianceNoLock<T> &stats_y() const noexcept {
        lock_guard lock(_mutex);
        return _state.stats_y();
    }

    // slope of least squares best fit, y = slope() * x + intercept(), or NAN
    // when less than 2 measurements
    T slope() const noexcept {
        lock_guard lock(_mutex);
        return _state.slope();
    }

    // intercept of least squares best fit, y = slope() * x + intercept(), or
    // NAN when less than 2 measurements
    T intercept() const noexcept {
        lock_guard lock(_mutex);
        return _state.intercept();
    }

    /** correlation of x and y, or NAN when less than 2 measurements */
    T correlation() const noexcept {
        lock_guard lock(_mutex);
        return _state.correlation();
    }

    // slope of least squares best fit trough (x,y), or NAN when no measurements
    T slope_through(T x, T y) const noexcept {
        lock_guard lock(_mutex);
        return _state.slope_through(x, y);
    }

    // slope of least squares best fit trough origin, or NAN when no
    // measurements
    T slope_through_origin() const noexcept {
        lock_guard lock(_mutex);
        return _state.slope_through_origin();
    }

    std::string toString(int precision = -1) const noexcept override {
        lock_guard lock(_mutex);
        return _state.toString(precision);
    }

  private:
    Internals::LinearRegressionNoLock<T> _state{};
    mutable M _mutex{};
};

} // namespace Metrics

#endif
