#pragma once

#include <cstddef>

namespace HDRI {

class debevec_weight final {
 public:
  debevec_weight() = delete;
  [[nodiscard]] static double get_weight(const int index) noexcept;
  [[nodiscard]] static std::size_t get_size() noexcept;
};

}  // namespace HDRI
