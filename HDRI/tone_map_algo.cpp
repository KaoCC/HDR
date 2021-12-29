#include "tone_map_algo.hpp"

#include <iostream>

namespace {

constexpr auto rgb_to_luminance(const float r, const float g, const float b) noexcept -> float {
  return 0.2126F * r + 0.7152F * g + 0.0722F * b;
}

}  // namespace

namespace HDRI {

auto reinhard_tone_map_algo(const cv::Mat &input_radiance) noexcept -> cv::Mat {
  constexpr auto epsilon = 0.00001F;
  constexpr auto a = 0.18F;  // from paper

  // color space transform
  cv::Mat lumi(input_radiance.size(), CV_32FC1);
  for (auto y = 0; y < input_radiance.size().height; ++y) {
    for (auto x = 0; x < input_radiance.size().width; ++x) {
      lumi.at<float>(y, x) =
          rgb_to_luminance(input_radiance.at<cv::Vec3f>(y, x)[2], input_radiance.at<cv::Vec3f>(y, x)[1],
                           input_radiance.at<cv::Vec3f>(y, x)[0]);
    }
  }

  auto Lw_bar = 0.0F;

  // loop over all values in the Mat
  for (auto y = 0; y < input_radiance.size().height; ++y) {
    for (auto x = 0; x < input_radiance.size().width; ++x) {
      Lw_bar += std::log(lumi.at<float>(y, x) + epsilon);  // from paper
    }
  }

  std::cerr << "Lw_bar: " << Lw_bar << std::endl;

  const auto N = static_cast<float>(input_radiance.total());

  // Equation 1 in the paper is wrong. The division by N should be placed
  // before the summation, not outside the exponentiation.
  Lw_bar = std::exp(Lw_bar / N);

  const auto coeff = a / Lw_bar;

  const auto L_white = 1.7F;  // test

  // compute Ld
  cv::Mat Ld(input_radiance.size(), CV_32FC1);

  for (auto y = 0; y < input_radiance.size().height; ++y) {
    for (auto x = 0; x < input_radiance.size().width; ++x) {
      auto L = coeff * lumi.at<float>(y, x);  // Ld = (a / Lw_bar ) * (Lw(x,y))
      Ld.at<float>(y, x) = L * (1.0F + L / (L_white * L_white)) / (1.0F + L);

      if (L > L_white) {
        Ld.at<float>(y, x) = 1;
      }
    }
  }

  cv::Mat output_image(input_radiance.size(), CV_8UC3);
  for (auto idx = 0; idx < 3; ++idx) {  // rgb
    for (auto y = 0; y < input_radiance.size().height; ++y) {
      for (auto x = 0; x < input_radiance.size().width; ++x) {
        output_image.at<cv::Vec3b>(y, x)[idx] = cv::saturate_cast<uchar>(
            input_radiance.at<cv::Vec3f>(y, x)[idx] * (Ld.at<float>(y, x) * 255.0 / lumi.at<float>(y, x)));
      }
    }
  }

  return output_image;
}

}  // namespace HDRI