#include "HDRImage.hpp"

#include <iostream>

void HDRI::HDRImage::computeRadiance(const std::vector<HDRI::RawImage> &imageFiles,
                                     const std::array<cv::Mat, 3> &gCurves, HDRI::WeightFunction &dwf,
                                     const std::vector<double> &expo) {

  mRadiance = constructRadiance(imageFiles, gCurves, dwf, expo);
}

auto HDRI::HDRImage::getRadiance() const -> cv::Mat { return mRadiance; }

void HDRI::HDRImage::setToneMappingAlgorithm(ToneMapAlgo *algo) { mAlgoSelect = algo; }

auto HDRI::HDRImage::getToneMappingResult() const -> cv::Mat {

  if (mAlgoSelect != nullptr) {
    return mAlgoSelect->toneMap(mRadiance);
  }

  std::cerr << "No algorithm selected\n";

  return cv::Mat();
}
