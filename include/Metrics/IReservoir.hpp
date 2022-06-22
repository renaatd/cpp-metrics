#ifndef METRICS_IRESERVOIR_HPP
#define METRICS_IRESERVOIR_HPP

#include "Snapshot.hpp"

namespace Metrics {
template <class T> class IReservoir {
  public:
    virtual void reset() = 0;
    virtual void update(T value) = 0;
    virtual unsigned size() const = 0;
    virtual unsigned samples() const = 0;
    virtual const T *data() const = 0;
    virtual Snapshot<T> getSnapshot() const = 0;
    virtual ~IReservoir() = default;
};

} // namespace Metrics

#endif
