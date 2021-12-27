#pragma once

#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <vector>

namespace HDRI {

template <typename weight_function>
[[nodiscard]] cv::Mat linear_least_square_solver(weight_function &&weighting,
                                                 const std::vector<std::vector<int>> &z_value,
                                                 const std::vector<double> &delta_time, const size_t range,
                                                 const int lambda) noexcept {
  // zij , shutter , w, g, lE
  // From the paper.

  /// NOTE: Ax = b
  /// z_value[i][j]: the pixel value of location i in image j

  const auto num_plxels{std::size(z_value)};
  const auto num_images{std::size(z_value[0UL])};

  cv::Mat A = cv::Mat::zeros(num_plxels * num_images + 1UL + (range - 1UL - 1UL), range + num_plxels, CV_64F);
  cv::Mat b = cv::Mat::zeros(A.size().height, 1, CV_64F);

  size_t current_row = 0UL;
  for (auto i = 0; i < num_plxels; ++i) {    // N  (pixel)
    for (auto j = 0; j < num_images; ++j) {  // P  (image)

      const auto w_ij = std::invoke(std::forward<weight_function>(weighting), z_value[i][j]);
      A.at<double>(current_row, z_value[i][j]) = w_ij;
      A.at<double>(current_row, range + i) = -w_ij;
      b.at<double>(current_row, 0) = w_ij * std::log(delta_time[j]);

      ++current_row;
    }
  }

  A.at<double>(current_row, 127) = 1;  // set the middle to Zero: g(127) = 0
  ++current_row;

  // get x !

  // w(z)g"(z) = w(z)(g(z - 1) -2g(z) + g(z + 1))
  for (auto z = 1UL; z <= range - 1UL - 1UL; ++z) {
    const auto result = std::invoke(std::forward<weight_function>(weighting), z);

    A.at<double>(current_row, z - 1) = lambda * result;
    A.at<double>(current_row, z) = -2.0 * lambda * result;
    A.at<double>(current_row, z + 1) = lambda * result;

    ++current_row;
  }

  cv::Mat x;
  cv::solve(A, b, x, cv::DECOMP_SVD);

  // get g and lE
  // g(z) : the log exposure corresponding to pixel value z

  return x;
}

}  // namespace HDRI
