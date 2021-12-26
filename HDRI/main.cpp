
#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>

#include "common.hpp"
#include "hdr_image.hpp"
#include "raw_image.hpp"
#include "tone_map_algo.hpp"

namespace {

// (1/shutter_speed)
/// NOTE: factor of two
constexpr std::array default_shutter_speed = {1 / 32, 1 / 16, 1 / 8, 1 / 4, 1 / 2, 1,   2,   4,
                                              8,      16,     32,    64,    128,   256, 512, 1024};

void output_curve(const cv::Mat &curve) noexcept {
  const auto width = curve.size().width;
  const auto height = curve.size().height;

  std::ofstream fout("out_curve.txt");

  for (auto q = 0UL; q < height; ++q) {
    for (auto p = 0UL; p < width; ++p) {
      fout << curve.at<double>(q, p) << std::endl;
    }
  }
}

std::vector<HDRI::raw_image> load_raw_images(const std::string &base_path, const std::string &file_name) noexcept {
  const auto full_path = base_path + file_name;

  std::ifstream input(full_path);

  if (!input) {
    std::cerr << "Cannot read input. Path: " << full_path << std::endl;
    return {};
  }

  std::string line;

  bool num_flag = false;
  std::size_t num_images = 0UL;
  std::vector<HDRI::raw_image> images;

  while (std::getline(input, line)) {
    std::stringstream line_splitter(line);
    char token{};

    line_splitter >> token;
    if (!line_splitter || token == '#') {
      continue;
    }

    line_splitter.putback(token);

    if (!num_flag) {
      std::size_t num{};
      line_splitter >> num;

      // first number
      num_images = num;
      std::cout << "Number of images: " << num_images << std::endl;
      num_flag = true;

      images.reserve(num_images);

    } else {
      std::string image_file_name;
      double inv_shutter_speed{};

      line_splitter >> image_file_name >> inv_shutter_speed;

      std::cout << "file: " << image_file_name << " inv shutter speed: " << inv_shutter_speed << std::endl;

      // read image
      images.emplace_back(base_path + image_file_name, inv_shutter_speed);

      if (std::size(images) == num_images) {
        break;
      }
    }
  }

  return images;
}

}  // namespace

int main(int argc, char *argv[]) {
  constexpr auto default_base_path{"../InputImage/"};
  constexpr auto default_files{"list.txt"};

  std::string base_path;
  if (argc > 1) {
    base_path = argv[1];
  } else {
    base_path = default_base_path;
  }

  std::string file_list;
  if (argc > 2) {
    file_list = argv[2];
  } else {
    file_list = default_files;
  }

  const auto start_time = std::chrono::high_resolution_clock::now();

  const auto raw_images = load_raw_images(base_path, file_list);
  if (raw_images.empty()) {
    std::cerr << "No input images" << std::endl;
    return EXIT_FAILURE;
  }

  /// HDR Image Creation

  std::cout << "Create HDR Image" << std::endl;

  HDRI::hdr_image hdr_image(raw_images);

  /// Tone Map

  std::cout << "Tone Mapping" << std::endl;
  const auto tone_mapping_image = hdr_image.compute_tone_mapping(HDRI::reinhard_tone_map_algo);

  const auto end_time = std::chrono::high_resolution_clock::now();
  const auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);

  std::cout << "Execution time : " << time_span.count() << "s\n";

  /// Output

  const auto &curves = hdr_image.get_curves();

  std::cout << "Output Curves" << std::endl;
  output_curve(curves[0UL]);

  constexpr auto output_radiance_file_name{"radiance.exr"};

  std::cout << "Output HDR radiance" << std::endl;

  try {
    cv::imwrite(output_radiance_file_name, hdr_image.get_radiance());
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
    std::exit(EXIT_FAILURE);
  }

  constexpr auto output_image_file_name{"tone_mapping_image.jpg"};

  std::cout << "Output Tone Mapping Image" << std::endl;

  try {
    cv::imwrite(output_image_file_name, tone_mapping_image);
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
    std::exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
