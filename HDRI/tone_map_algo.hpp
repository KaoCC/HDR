#pragma once

#include <opencv2/core/mat.hpp>

namespace HDRI {

[[nodiscard]] cv::Mat reinhard_tone_map_algo(const cv::Mat &input_radiance) noexcept;

}  // namespace HDRI
