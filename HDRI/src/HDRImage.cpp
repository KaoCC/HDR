#include "HDRImage.hpp"

#include <iostream>

void HDRI::HDRImage::computeRadiance(const std::vector<HDRI::RawImage>& imageFiles, const std::array<cv::Mat, 3>& gCurves, HDRI::WeightFunction & dwf, const std::vector<double>& expo) {


	mRadiance = constructRadiance(imageFiles, gCurves, dwf, expo);

}

cv::Mat HDRI::HDRImage::getRadiance() const {
	return mRadiance;
}

void HDRI::HDRImage::setToneMappingAlgorithm(ToneMapAlgo * algo) {
	mAlgoSelect = algo;
}

cv::Mat HDRI::HDRImage::getToneMappingResult() const {

	if (mAlgoSelect) {
		return mAlgoSelect->toneMap(mRadiance);
	} else {

		std::cerr << "No algorithm selected\n";

		return cv::Mat();
	}

}


