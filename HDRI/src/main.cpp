


#include <vector>


#include "rawImage.hpp"

#include "DebevecWeight.hpp"
#include "Common.hpp"

#include "LinearLeastSquares.hpp"
#include "HDRImage.hpp"

#include "ReinhardAlgo.hpp"

#include <iostream>
#include <cstdint>

#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <array>

const std::string kDefaultBasePath = "../InputImage/";
const std::string kDefaultFileList = "list.txt";



// (1/shutter_speed)
// Note: factor of two
static const double defaultShutterSpeed[] = { 1 / 32, 1 / 16, 1 / 8, 1 / 4, 1 / 2, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };




// tmp
static void getAccurateExposure();


static void outputCurve(const cv::Mat& curve) {
	size_t tmpW = curve.size().width;
	size_t tmpH = curve.size().height;

	std::ofstream fout("out.txt");

	for (int q = 0; q < tmpH; ++q) {
		for (int p = 0; p < tmpW; ++p) {
			fout << curve.at<double>(q, p) << std::endl;
		}
	}
}


static std::vector<cv::Mat> shrinkImages(const std::vector<HDRI::RawImage>&in) {

	std::vector<cv::Mat> out;

	const size_t kRatio = 50;

	for (const auto& img : in) {

		const auto& ref = img.getImageData();

		cv::Mat shrinkMat;
		cv::resize(ref, shrinkMat, cv::Size(ref.cols, ref.rows));
		cv::resize(shrinkMat, shrinkMat, cv::Size(ref.cols / kRatio, ref.rows / kRatio));

		out.push_back(shrinkMat);
	}


	return out;

}


static std::vector<std::vector<PixelData>> generateRawPixelData(const std::vector<cv::Mat>& shrinkMat) {

	size_t width = shrinkMat[0].size().width;
	size_t height = shrinkMat[0].size().height;

	std::vector<std::vector<PixelData>> pixelRaw(shrinkMat.size());

	for (size_t idx = 0; idx < shrinkMat.size(); ++idx) {

		pixelRaw[idx].resize(width * height);

		for (size_t y = 0; y < height; ++y) {
			for (size_t x = 0; x < width; ++x) {

				pixelRaw[idx][y * width + x].b = shrinkMat[idx].at<cv::Vec3b>(y, x)[0];
				pixelRaw[idx][y * width + x].g = shrinkMat[idx].at<cv::Vec3b>(y, x)[1];
				pixelRaw[idx][y * width + x].r = shrinkMat[idx].at<cv::Vec3b>(y, x)[2];
			}
		}

	}

	return pixelRaw;
}


static std::array<std::vector<std::vector<int>>, 3> convertToZ(const std::vector<std::vector<PixelData>>& pixelRaw, const size_t imageSize, const size_t numOfImage) {

	std::array<std::vector<std::vector<int>>, 3> Z;		// r, g, b


	for (auto& zColors : Z) {
		zColors.resize(imageSize);
	}


	for (size_t i = 0; i < imageSize; ++i) {			// image pixel

		Z[0][i].resize(numOfImage);
		Z[1][i].resize(numOfImage);
		Z[2][i].resize(numOfImage);

		for (size_t j = 0; j < numOfImage; ++j) {		// num of iamge
			Z[0][i][j] = pixelRaw[j][i].b;
			Z[1][i][j] = pixelRaw[j][i].g;
			Z[2][i][j] = pixelRaw[j][i].r;
		}

	}

	return Z;


}



int main() {

	std::vector<HDRI::RawImage> imageFiles;


	try {
		loadRawImages(kDefaultBasePath, kDefaultFileList, imageFiles);
	} catch (std::exception& e) {
		std::cerr << e.what() << '\n';
		std::exit(-1);
	}


	HDRI::DebevecWeight dwf;


	auto shrinkMat = shrinkImages(imageFiles);
	std::vector<std::vector<PixelData>> pixelRaw = generateRawPixelData(shrinkMat);

	std::cerr << "Convert\n";

	// convert
	auto Z = convertToZ(pixelRaw, shrinkMat[0].total(), shrinkMat.size());

	// set exp
	std::vector<double> expo;
	for (const auto& img : imageFiles) {
		expo.push_back(img.getExposure());
	}


	const int lambda = 10;
	std::array<cv::Mat, 3> gCurves;		// R, G, B
	for (size_t c = 0; c < Z.size(); ++c) {
		HDRI::LinearLeastSquares::solver(Z[c], expo, dwf, lambda, gCurves[c]);
	}

	std::cerr << "Done\n";

	// test
	outputCurve(gCurves[0]);

	std::cerr << "Compute radiance\n";
	// radiance ?
	//cv::Mat hdrImg = constructRadiance(imageFiles, gCurves, dwf, expo);

	HDRI::HDRImage hdrImage;
	hdrImage.computeRadiance(imageFiles, gCurves, dwf, expo);


	try {
		cv::imwrite("radiance.hdr", hdrImage.getRadiance());
	} catch (std::exception& e) {
		std::cerr << e.what() << '\n';
		std::exit(-1);
	}



	//tone map

	std::cerr << "tone map\n";

	HDRI::ReinhardAlgo rAlgo;

	hdrImage.setToneMappingAlgorithm(&rAlgo);
	auto outimage = hdrImage.getToneMappingResult();


	try {
		cv::imwrite("output_image.jpg", outimage);
	} catch (std::exception& e) {
		std::cerr << e.what() << '\n';
		std::exit(-1);
	}



	return 0;
}



