#ifndef METRICS_IRESERVOIR_HPP
#define METRICS_IRESERVOIR_HPP

#include "Snapshot.hpp"

namespace Metrics {
template <class T> class IReservoir {
  public:
    virtual void reset() noexcept = 0;
    virtual void update(T value) noexcept = 0;
    virtual unsigned size() const noexcept = 0;
    virtual unsigned samples() const noexcept = 0;
    virtual const T *data() const noexcept = 0;
    virtual Snapshot<T> getSnapshot() const noexcept = 0;
    virtual ~IReservoir() = default;
};

} // namespace Metrics

#endif
