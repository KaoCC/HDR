#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <cstdint>
#include <opencv2/core/mat.hpp>


#include "rawImage.hpp"
#include "WeightFunction.hpp"

struct PixelData {
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;
};


void loadRawImages(const std::string& basePath, const std::string& fileName, std::vector<HDRI::RawImage>& images);
cv::Mat constructRadiance(const std::vector<HDRI::RawImage>& imageFiles, const std::array<cv::Mat, 3>& gCurves, HDRI::WeightFunction& dwf, const std::vector<double> expo);




#endif

