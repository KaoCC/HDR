

#pragma once

#include <opencv2/core/mat.hpp>

namespace HDRI {

class raw_image final {
 public:
  explicit raw_image(std::string file_name, const double shutter_speed);

  raw_image(const raw_image &) noexcept = default;
  raw_image(raw_image &&) noexcept = default;
  raw_image &operator=(const raw_image &) noexcept = default;
  raw_image &operator=(raw_image &&) noexcept = default;
  ~raw_image() = default;

  [[nodiscard]] size_t get_total_size() const noexcept;

  [[nodiscard]] int get_width() const noexcept;
  [[nodiscard]] int get_height() const noexcept;

  [[nodiscard]] const cv::Mat &get_image_data() const noexcept;

  [[nodiscard]] double get_exposure() const noexcept;

 private:
  cv::Mat image_data;
  std::string name;
  double expo;
};

}  // namespace HDRI
