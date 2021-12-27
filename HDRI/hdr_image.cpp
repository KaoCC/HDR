
#include "hdr_image.hpp"

#include <future>
#include <opencv2/imgproc.hpp>

#include "common.hpp"
#include "debevec_weight.hpp"
#include "linear_least_square_solver.hpp"
#include "raw_image.hpp"

namespace {

template <typename weight_function>
cv::Mat construct_radiance(const std::vector<HDRI::raw_image> &image_files, const std::array<cv::Mat, 3UL> &curves,
                           weight_function &&weighting, const std::vector<double> &expo) noexcept {
  // Size
  const auto width = image_files[0UL].get_width();
  const auto height = image_files[0UL].get_height();

  // Radiance
  cv::Mat radiance(height, width, CV_32FC3);

  for (auto idx = 0UL; idx < std::size(curves); ++idx) {  // r, g, b
    for (auto y = 0UL; y < height; ++y) {
      for (auto x = 0UL; x < width; ++x) {
        double weighted_sum = 0.0;
        double result = 0.0;

        // loop over all images
        for (auto k = 0UL; k < std::size(image_files); ++k) {
          const auto color = static_cast<int>(image_files[k].get_image_data().at<cv::Vec3b>(y, x)[idx]);
          const double w = std::invoke(std::forward<weight_function>(weighting), color);

          result += static_cast<double>(w * (curves[idx].at<double>(color, 0) - std::log(expo[k])));
          weighted_sum += w;
        }

        if (weighted_sum < std::numeric_limits<double>::epsilon() &&
            weighted_sum > -std::numeric_limits<double>::epsilon()) {  // near 0.0
          radiance.at<cv::Vec3f>(y, x)[idx] = 0;
        } else {
          radiance.at<cv::Vec3f>(y, x)[idx] = std::exp(result / weighted_sum);
        }
      }
    }
  }

  return radiance;
}

std::vector<cv::Mat> shrink_images(const std::vector<HDRI::raw_image> &input) noexcept {
  std::vector<cv::Mat> out;
  out.reserve(std::size(input));

  constexpr size_t ratio = 50UL;

  for (const auto &img : input) {
    const auto &image_data = img.get_image_data();

    size_t resized_col = image_data.cols / ratio;
    size_t resized_row = image_data.rows / ratio;

    if (resized_col < 15UL) {
      resized_col = 15UL;
    }

    if (resized_row < 15UL) {
      resized_row = 15UL;
    }

    cv::Mat shrink_mat;
    cv::resize(image_data, shrink_mat, cv::Size(image_data.cols, image_data.rows));
    cv::resize(shrink_mat, shrink_mat, cv::Size(resized_col, resized_row));

    out.push_back(shrink_mat);
  }

  return out;
}

std::vector<std::vector<pixel>> generate_raw_pixel(const std::vector<cv::Mat> &shrink_mat) noexcept {
  const auto width = shrink_mat[0UL].size().width;
  const auto height = shrink_mat[0UL].size().height;

  std::vector<std::vector<pixel>> pixels(shrink_mat.size());

  for (auto idx = 0UL; idx < shrink_mat.size(); ++idx) {
    pixels[idx].resize(width * height);

    for (auto y = 0UL; y < height; ++y) {
      for (auto x = 0UL; x < width; ++x) {
        pixels[idx][y * width + x].b = shrink_mat[idx].at<cv::Vec3b>(y, x)[0UL];
        pixels[idx][y * width + x].g = shrink_mat[idx].at<cv::Vec3b>(y, x)[1UL];
        pixels[idx][y * width + x].r = shrink_mat[idx].at<cv::Vec3b>(y, x)[2UL];
      }
    }
  }

  return pixels;
}

std::array<std::vector<std::vector<int>>, 3UL> convert_to_z(const std::vector<std::vector<pixel>> &pixel,
                                                            const size_t image_size, const size_t num_images) noexcept {
  std::array<std::vector<std::vector<int>>, 3UL> z_values;  // r, g, b

  for (auto &z_colors : z_values) {
    z_colors.resize(image_size);
  }

  for (auto i = 0UL; i < image_size; ++i) {  // number of pixels in an image

    z_values[0UL][i].resize(num_images);
    z_values[1UL][i].resize(num_images);
    z_values[2UL][i].resize(num_images);

    for (auto j = 0UL; j < num_images; ++j) {  // number of images
      z_values[0UL][i][j] = pixel[j][i].b;
      z_values[1UL][i][j] = pixel[j][i].g;
      z_values[2UL][i][j] = pixel[j][i].r;
    }
  }

  return z_values;
}

}  // namespace

namespace HDRI {

hdr_image::hdr_image(const std::vector<raw_image> &raw_images) {
  // set exposure
  std::vector<double> exposure;
  exposure.reserve(std::size(raw_images));

  for (const auto &raw_image : raw_images) {
    exposure.push_back(raw_image.get_exposure());
  }

  compute_curves(raw_images, exposure);
  compute_radiance(raw_images, exposure);
}

const std::array<cv::Mat, 3UL> &hdr_image::get_curves() const noexcept { return curves; }

void hdr_image::compute_curves(const std::vector<HDRI::raw_image> &raw_images,
                               const std::vector<double> &exposure) noexcept {
  const auto shrink_mat = shrink_images(raw_images);
  std::vector<std::vector<pixel>> raw_pixel = generate_raw_pixel(shrink_mat);

  // convert
  const auto z_values = convert_to_z(raw_pixel, shrink_mat[0UL].total(), shrink_mat.size());

  constexpr auto lambda = 10;

  const auto weight_function = [](const auto color) { return debevec_weight::get_weight(color); };

  std::array<std::future<cv::Mat>, 3UL> futures;
  for (auto c = 0U; c < std::size(z_values); ++c) {
    futures[c] = std::async(std::launch::async, linear_least_square_solver<decltype(weight_function)>, weight_function,
                            z_values[c], exposure, debevec_weight::get_size(), lambda);
  }

  for (auto c = 0U; c < std::size(futures); ++c) {
    curves[c] = futures[c].get();
  }
}

void hdr_image::compute_radiance(const std::vector<HDRI::raw_image> &raw_images,
                                 const std::vector<double> &exposure) noexcept {
  radiance = construct_radiance(raw_images, curves, debevec_weight::get_weight, exposure);
}

const cv::Mat &hdr_image::get_radiance() const noexcept { return radiance; }

}  // namespace HDRI