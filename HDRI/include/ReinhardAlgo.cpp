#include "ReinhardAlgo.hpp"



cv::Mat HDRI::ReinhardAlgo::toneMap(const cv::Mat& inputRadiance) {


	const double epsilon = 0.001;
	const double a = 0.18;		// from paper


	double Lw_bar = 0;


	// color space transform
	cv::Mat lumi(inputRadiance.size(), CV_64FC1);
	for (size_t y = 0; y < inputRadiance.size().height; ++y) {
		for (size_t x = 0; x < inputRadiance.size().width; ++x) {

			// L = 0.2126 * R + 0.7152 * G + 0.0722 * B;
			lumi.at<double>(y, x) = 0.2126 * inputRadiance.at<cv::Vec3f>(y, x)[2] + 0.7152 * inputRadiance.at<cv::Vec3f>(y, x)[1] + 0.0722 * inputRadiance.at<cv::Vec3f>(y, x)[0];
		}
	}


	// loop over all values in the Mat
	for (size_t y = 0; y < inputRadiance.size().height; ++y) {
		for (size_t x = 0; x < inputRadiance.size().width; ++x) {

			Lw_bar += std::log(lumi.at<double>(y, x) + epsilon);		// from paper

		}
	}

	const size_t N = inputRadiance.total();

	// Equation 1 in the paper is wrong. The division by N should be placed before the summation, not outside the exponentiation.
	Lw_bar = std::exp(Lw_bar / N);


	double coeff = a / Lw_bar;


	double L_white = 1.5;		// test

	// compute Ld
	cv::Mat Ld(inputRadiance.size(), CV_64FC1);

	for (size_t y = 0; y < inputRadiance.size().height; ++y) {
		for (size_t x = 0; x < inputRadiance.size().width; ++x) {

			double L = coeff * lumi.at<double>(y, x);		// Ld = (a / Lw_bar ) * (Lw(x,y))
			Ld.at<double>(y, x) = L * (1 + L / (L_white) * (L_white)) / (1.0 + L);

		}
	}




	cv::Mat outputImage(inputRadiance.size(), CV_8UC3);
	for (size_t idx = 0; idx < 3; ++idx) {		// rgb

		for (size_t y = 0; y < inputRadiance.size().height; ++y) {
			for (size_t x = 0; x < inputRadiance.size().width; ++x) {

				outputImage.at<cv::Vec3b>(y, x)[idx] = cv::saturate_cast<uchar>(inputRadiance.at<cv::Vec3f>(y, x)[idx] * (Ld.at<double>(y, x) * 255.0 / lumi.at<double>(y, x)));

			}
		}

	}

	return outputImage;

}


