#include "debevec_weight.hpp"

namespace {
constexpr int z_min = 0;
constexpr int z_max = 255;
constexpr int size = z_max - z_min + 1;
constexpr auto z_mid = static_cast<int>(0.5 * (z_max + z_min));
}  // namespace

namespace HDRI {

auto debevec_weight::get_size() noexcept -> std::size_t { return size; }

auto debevec_weight::get_weight(const int z_value) noexcept -> double {
  // cap ?

  if (z_value < z_min || z_value > z_max) {
    return 0.0;
  }

  if (z_value <= z_mid) {
    return z_value - z_min;
  }

  return z_max - z_value;
}

}  // namespace HDRI
