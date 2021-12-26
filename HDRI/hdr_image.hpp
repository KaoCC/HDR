
#pragma once

#include <array>
#include <opencv2/core/mat.hpp>

namespace HDRI {
class raw_image;

class hdr_image final {
 public:
  explicit hdr_image(const std::vector<raw_image> &raw_images);

  [[nodiscard]] const cv::Mat &get_radiance() const noexcept;
  [[nodiscard]] const std::array<cv::Mat, 3UL> &get_curves() const noexcept;

  template <typename mapping_function>
  [[nodiscard]] cv::Mat compute_tone_mapping(mapping_function &&algo) const noexcept;

 private:
  void compute_curves(const std::vector<raw_image> &raw_images, const std::vector<double> &expo) noexcept;
  void compute_radiance(const std::vector<raw_image> &raw_images, const std::vector<double> &expo) noexcept;

  std::array<cv::Mat, 3UL> curves;
  cv::Mat radiance;
};

template <typename mapping_function>
cv::Mat hdr_image::compute_tone_mapping(mapping_function &&algo) const noexcept {
  return std::invoke(std::forward<mapping_function>(algo), radiance);
}

}  // namespace HDRI
