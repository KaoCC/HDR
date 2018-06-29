
#include <memory>

#include "LinearLeastSquares.hpp"



void HDRI::LinearLeastSquares::solver(const std::vector<std::vector<int>>& Z, const std::vector<double>& deltaT, const WeightFunction& wf, int lambda, cv::Mat& result) {	// zij , shutter , w, g, lE


	//tmp
	//int lambda = 1;

	// From the paper.

	// Note : Ax = b

	auto n = wf.getSize();

	//std::vector<std::vector<double>> A;

	//std::vector<double> b;
	//b.resize(A.size());



	cv::Mat A = cv::Mat::zeros(Z.size() * Z[0].size() + n + 1, n + Z.size(), CV_64F);
	cv::Mat b = cv::Mat::zeros(A.size().height, 1, CV_64F);


	int k = 0;

	for (auto i = 0; i < Z.size(); ++i) {		// N  (pixel)
		for (auto j = 0; j < Z[i].size(); ++j) {		// P  (image)

			double w_ij = wf.getWeight(Z[i][j]);  // +1 ??
			A.at<double>(k, Z[i][j]) = w_ij;
			A.at<double>(k, n + i) = -w_ij;
			b.at<double>(k, 0) = w_ij * std::log(deltaT[j]);

			++k;
		}

	}

	A.at<double>(k, 128) = 1;		// set the middle to Zero 
	++k;


	// get x !

	// g(z - 1) -2g(z) + g(z + 1)
	for (auto i = 0; i < n - 1; ++i) {

		A.at<double>(k, i) = lambda * wf.getWeight(i + 1);
		A.at<double>(k, i + 1) = -2 * lambda * wf.getWeight(i + 1);
		A.at<double>(k, i + 2) = lambda * wf.getWeight(i + 1);
		++k;
	}

	cv::Mat x(A.rows, 1, CV_64F);
	cv::solve(A, b, x, cv::DECOMP_SVD);

	// get g and lE

	result = std::move(x);

	//...
}

