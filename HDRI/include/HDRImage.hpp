#ifndef _HDRI_HDRIMAGE_HPP_
#define _HDRI_HDRIMAGE_HPP_

#include <opencv2/core/mat.hpp>

#include "Common.hpp"
#include "ToneMapAlgo.hpp"
#include "WeightFunction.hpp"
#include "rawImage.hpp"

namespace HDRI {

class HDRImage {

public:
  HDRImage() = default;

  void computeRadiance(const std::vector<HDRI::RawImage> &imageFiles, const std::array<cv::Mat, 3> &gCurves,
                       HDRI::WeightFunction &dwf, const std::vector<double> &expo);
  [[nodiscard]] cv::Mat getRadiance() const;

  void setToneMappingAlgorithm(ToneMapAlgo *algo);

  [[nodiscard]] cv::Mat getToneMappingResult() const;

private:
  cv::Mat mRadiance;

  ToneMapAlgo *mAlgoSelect = nullptr;
};

} // namespace HDRI

#endif
