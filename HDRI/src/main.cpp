



#include <vector>
#include <string>

#include <sstream>
#include <fstream> 

#include "rawImage.hpp"

#include "DebevecWeight.h"
#include "Common.hpp"

#include "LinearLeastSquares.hpp"

#include <iostream>
#include <cstdint>

#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>

const std::string kDefaultBasePath = "../Resource/";
const std::string kDefaultFileList = "list.txt";



// (1/shutter_speed)
// Note: factor of two
static const double defaultShutterSpeed[] = {1/32, 1/16, 1/8, 1/4, 1/2, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};


static void loadRawImages(const std::string& basePath, const std::string& fileName, std::vector<HDRI::RawImage>& images) {

	const std::string fullPath = basePath + fileName;

	std::ifstream inputStream(fullPath);

	if (!inputStream) {
		throw std::runtime_error("Could not open file: " + fullPath);
	}

	std::string lineString;


	bool numFlag = false;

	std::size_t numOfImages = 0;
	std::size_t currentImageIndex = 0;

	while (std::getline(inputStream, lineString)) {

		std::stringstream lineSplitter(lineString);
		char token;

		lineSplitter >> token;
		if ((!lineSplitter) || token == '#') {
			continue;
		} else {
			lineSplitter.putback(token);
		}



		if (!numFlag) {

			std::size_t num = 0;
			lineSplitter >> num;

			// first number
			numOfImages = num;
			std::cout << "Number of image: " << numOfImages << '\n';
			numFlag = true;

			images.resize(numOfImages);


		} else {

			std::string imageFileName;
			double invShutterSpeed;

			lineSplitter >> imageFileName >> invShutterSpeed;

			std::cout << "file: " << imageFileName << " shutter: " << invShutterSpeed << std::endl;


			// read image
			images[currentImageIndex++].load(basePath + imageFileName, invShutterSpeed);

			if (currentImageIndex == numOfImages) {
				break;
			}

		}


	}


}

// tmp
static void getAccurateExposure();


static std::vector<cv::Mat> shrinkImages(const std::vector<HDRI::RawImage>&in) {

	std::vector<cv::Mat> out;

	for (const auto& img : in) {

		const auto& ref = img.getImageData();

		cv::Mat shrinkMat;
		cv::resize(ref, shrinkMat, cv::Size(ref.cols, ref.rows));
		cv::resize(shrinkMat, shrinkMat, cv::Size(ref.cols / 10, ref.rows / 10));

		out.push_back(shrinkMat);
	}


	return out;

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

	//w.getWeight(0);


	auto shrinkMat = shrinkImages(imageFiles);

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

	std::cerr << "Convert\n";

	// convert
	
	std::vector<std::vector<int>> Z_r;		// Zij : i pixel, j image
	std::vector<std::vector<int>> Z_g;
	std::vector<std::vector<int>> Z_b;

	Z_r.resize(shrinkMat[0].total());
	Z_g.resize(shrinkMat[0].total());
	Z_b.resize(shrinkMat[0].total());

	size_t imageSz = Z_r.size();

	for (size_t i = 0; i < imageSz; ++i) {			// image pixel

		const size_t numOfImage = shrinkMat.size();

		Z_r[i].resize(numOfImage);
		Z_g[i].resize(numOfImage);
		Z_b[i].resize(numOfImage);

		for (size_t j = 0; j < numOfImage; ++j) {		// num of iamge
			Z_r[i][j] = pixelRaw[j][i].r;
			Z_g[i][j] = pixelRaw[j][i].g;
			Z_b[i][j] = pixelRaw[j][i].b;
		}

	}
	

	std::vector<double> expo;
	std::vector<cv::Mat> gCurve(3);		// R, G, B
	
	// set exp
	for (const auto& img : imageFiles) {
		expo.push_back(img.getExposure());
	}
	


	HDRI::LinearLeastSquares::solver(Z_r, expo, dwf, gCurve[2]);		//tmp
	HDRI::LinearLeastSquares::solver(Z_g, expo, dwf, gCurve[1]);
	HDRI::LinearLeastSquares::solver(Z_b, expo, dwf, gCurve[0]);

	std::cerr << "Done\n";


	size_t tmpW = gCurve[0].size().width;
	size_t tmpH = gCurve[0].size().height;

	std::ofstream fout("out.txt");

	for (int q = 0; q < tmpH; ++q) {
		for (int p = 0; p < tmpW; ++p) {
			fout << gCurve[0].at<double>(q, p) << std::endl;
		}
	}

	
	// Size ?

	width = imageFiles[0].getWidth();
	height = imageFiles[0].getHeight();

	// radiance ?

	cv::Mat hdr_img = cv::Mat(height, width, CV_32FC3);


	for (size_t idx = 0; idx < gCurve.size(); ++idx) {

		for (size_t y = 0; y < height; ++y) {
			for (size_t x = 0; x < width; ++x) {

				double weightedSum = 0.0;
				double result = 0.0;

				// loop over all imgs

				for (size_t k = 0; k < imageFiles.size(); ++k) {

					int color = (int)imageFiles[k].getImageData().at<cv::Vec3b>(y, x)[idx];
					double w = dwf.getWeight(color);

					result += (double)(w * (gCurve[idx].at<double>(color, 0) - expo[k]));
					weightedSum += w;

				}


				hdr_img.at<cv::Vec3f>(y, x)[idx] = std::exp(result / weightedSum);

			}
		}


	}


	try {
		cv::imwrite("alpha.hdr", hdr_img);
	} catch (std::exception& ex) {
		fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
		return 1;
	}


	return 0;
}



