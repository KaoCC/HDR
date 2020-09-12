
#ifndef _DEBEVEC_WEIGHT_HPP_
#define _DEBEVEC_WEIGHT_HPP_

#include "WeightFunction.hpp"

#include <vector>

namespace HDRI {

class DebevecWeight : public WeightFunction {

public:
  DebevecWeight();
  [[nodiscard]] double getWeight(int index) const override;
  [[nodiscard]] std::size_t getSize() const override;

private:
  static const int kZmin;
  static const int kZmax;
  static const int N;

  std::vector<std::uint8_t> mTable;
};

} // namespace HDRI

#endif
