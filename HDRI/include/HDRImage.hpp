#ifndef _HDRI_HDRIMAGE_HPP_
#define _HDRI_HDRIMAGE_HPP_

#include <opencv2/core/mat.hpp>

#include "ToneMapAlgo.hpp"
#include "rawImage.hpp"
#include "WeightFunction.hpp"
#include "Common.hpp"

namespace HDRI {


	class HDRImage {

	public:

		HDRImage() = default;

		void computeRadiance(const std::vector<HDRI::RawImage>& imageFiles, const std::array<cv::Mat, 3>& gCurves, HDRI::WeightFunction& dwf, const std::vector<double>& expo);
		cv::Mat getRadiance() const;


		void setToneMappingAlgorithm(ToneMapAlgo* algo);

		cv::Mat getToneMappingResult() const;

	private:

		cv::Mat mRadiance;
		

		ToneMapAlgo* mAlgoSelect = nullptr;


	};



}










#endif
