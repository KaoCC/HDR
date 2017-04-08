#ifndef _HDRI_LINEARLEASTSQARES_HPP_
#define _HDRI_LINEARLEASTSQARES_HPP_


#include "WeightFunction.hpp"
#include <opencv2\core\mat.hpp>

#include "rawImage.hpp"

#include "Common.hpp"

namespace HDRI {


	class LinearLeastSquares {

	public:

		LinearLeastSquares() = delete;

		static void solver(const std::vector<std::vector<int>> Z, const std::vector<double> deltaT,const WeightFunction& wf, cv::Mat& gFunction, cv::Mat& lE);


	};

}










#endif

