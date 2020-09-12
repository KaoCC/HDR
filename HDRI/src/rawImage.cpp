#include "rawImage.hpp"
#include <opencv2/imgcodecs.hpp>

#include <iostream>

namespace HDRI {

void RawImage::load(const std::string &fileName, double ss) {

  mImageData = cv::imread(fileName);
  expo = 1 / ss;
  mName = fileName;

  if (mImageData.empty()) {
    std::cerr << "Fail to load: " + fileName << std::endl;
    throw std::runtime_error("Fail to load" + fileName);
  }
  std::cerr << "Loaded: " << fileName << std::endl;
}

auto RawImage::getTotalSize() const -> size_t { return mImageData.total(); }

auto RawImage::getWidth() const -> int { return mImageData.size().width; }

auto RawImage::getHeight() const -> int { return mImageData.size().height; }

auto RawImage::getImageData() const -> const cv::Mat & { return mImageData; }

auto RawImage::getExposure() const -> double { return expo; }

} // namespace HDRI
