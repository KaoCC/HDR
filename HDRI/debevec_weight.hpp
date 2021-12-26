#pragma once

#include <vector>

namespace HDRI {

class debevec_weight final {
 public:
  debevec_weight();
  [[nodiscard]] double get_weight(const int index) const noexcept;
  [[nodiscard]] std::size_t get_size() const noexcept;

 private:
  std::vector<std::uint8_t> table;
};

}  // namespace HDRI
