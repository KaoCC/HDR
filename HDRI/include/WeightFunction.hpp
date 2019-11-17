
#ifndef _HDRI_WEIGHTFUNCTION_HPP_
#define _HDRI_WEIGHTFUNCTION_HPP_

#include <cstddef>

namespace HDRI {

class WeightFunction {

  public:
    virtual double getWeight(int index) const = 0;
    virtual std::size_t getSize() const = 0;
};

} // namespace HDRI

#endif
