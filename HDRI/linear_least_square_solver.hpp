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

  const auto n = range;

  cv::Mat A = cv::Mat::zeros(z_value.size() * z_value[0UL].size() + n + 1, n + z_value.size(), CV_64F);
  cv::Mat b = cv::Mat::zeros(A.size().height, 1, CV_64F);

  int k = 0;
  for (auto i = 0; i < z_value.size(); ++i) {       // N  (pixel)
    for (auto j = 0; j < z_value[i].size(); ++j) {  // P  (image)

      const auto w_ij = std::invoke(std::forward<weight_function>(weighting), z_value[i][j]);
      A.at<double>(k, z_value[i][j]) = w_ij;
      A.at<double>(k, n + i) = -w_ij;
      b.at<double>(k, 0) = w_ij * std::log(delta_time[j]);

      ++k;
    }
  }

  A.at<double>(k, 128) = 1;  // set the middle to Zero
  ++k;

  // get x !

  // g(z - 1) -2g(z) + g(z + 1)
  for (auto i = 0; i < n - 1; ++i) {
    const auto result = std::invoke(std::forward<weight_function>(weighting), i + 1);

    A.at<double>(k, i) = lambda * result;
    A.at<double>(k, i + 1) = -2 * lambda * result;
    A.at<double>(k, i + 2) = lambda * result;

    ++k;
  }

  cv::Mat x;
  cv::solve(A, b, x, cv::DECOMP_SVD);

  // get g and lE

  return x;
}

}  // namespace HDRI
