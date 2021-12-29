#include "raw_image.hpp"

#include <iostream>
#include <opencv2/imgcodecs.hpp>

namespace HDRI {

raw_image::raw_image(const std::string& file_name, const double shutter_speed)
    : image_data{cv::imread(file_name)}, expo{1.0 / shutter_speed} {
  if (image_data.empty()) {
    std::cerr << "Fail to load: " + file_name << std::endl;
    throw std::runtime_error("Fail to load" + file_name);
  }

  std::cerr << "Loaded: " << file_name << std::endl;
}

auto raw_image::get_total_size() const noexcept -> size_t { return image_data.total(); }

auto raw_image::get_width() const noexcept -> int { return image_data.size().width; }

auto raw_image::get_height() const noexcept -> int { return image_data.size().height; }

auto raw_image::get_image_data() const noexcept -> const cv::Mat& { return image_data; }

auto raw_image::get_exposure() const noexcept -> double { return expo; }

}  // namespace HDRI
