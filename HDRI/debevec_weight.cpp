#include "debevec_weight.hpp"

namespace {
constexpr int z_min = 0;
constexpr int z_max = 255;
constexpr int range = z_max - z_min + 1;
}  // namespace

namespace HDRI {

debevec_weight::debevec_weight() {
  table.resize(range);
  for (auto i = 0; i < range; ++i) {
    if (i <= (0.5 * (z_max + z_min))) {
      table[i] = static_cast<std::uint8_t>(i - z_min);
    } else {
      table[i] = static_cast<std::uint8_t>(z_max - i);
    }
  }
}

auto debevec_weight::get_size() const noexcept -> size_t { return range; }

auto debevec_weight::get_weight(int index) const noexcept -> double {
  // cap ?

  if (index < z_min || index > z_max) {
    return 0;
  }
  return table[index];
}

}  // namespace HDRI
