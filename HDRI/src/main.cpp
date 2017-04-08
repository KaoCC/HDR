



#include <vector>
#include <string>

#include <sstream>
#include <fstream> 

#include "rawImage.hpp"

#include "DebevecWeight.h"
#include "Common.hpp"

#include <iostream>
#include <cstdint>

#include <opencv2/core/mat.hpp>


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



int main() {

	std::vector<HDRI::RawImage> imageFiles;


	try {
		loadRawImages(kDefaultBasePath, kDefaultFileList, imageFiles);
	} catch (std::exception& e) {
		std::cerr << e.what() << '\n';
		std::exit(-1);
	}

	
	HDRI::DebevecWeight w;

	//w.getWeight(0);




	size_t width = imageFiles[0].getWidth();
	size_t height = imageFiles[0].getHeight();


	std::vector<std::vector<PixelData>> pixelRaw(imageFiles.size());

	for (size_t idx = 0; idx < imageFiles.size(); ++idx) {


		for (size_t y = 0; y < height; ++y) {
			for (size_t x = 0; x < width; ++x) {

				pixelRaw[idx][y * width + x].b = imageFiles[idx].getImageData().at<cv::Vec3b>(y, x)[0];
				pixelRaw[idx][y * width + x].g = imageFiles[idx].getImageData().at<cv::Vec3b>(y, x)[1];
				pixelRaw[idx][y * width + x].r = imageFiles[idx].getImageData().at<cv::Vec3b>(y, x)[2];
			}
		}

	}

	// convert
	
	std::vector<std::vector<int>> Z_r;		// Zij : i pixel, j image
	std::vector<std::vector<int>> Z_g;
	std::vector<std::vector<int>> Z_b;

	Z_r.resize(imageFiles[0].getTotalSize());
	Z_g.resize(imageFiles[0].getTotalSize());
	Z_b.resize(imageFiles[0].getTotalSize());

	size_t imageSz = Z_r.size();

	for (size_t i = 0; i < imageSz; ++i) {			// image pixel

		const size_t numOfImage = imageFiles.size();

		Z_r[i].resize(numOfImage);
		Z_g[i].resize(numOfImage);
		Z_b[i].resize(numOfImage);

		for (size_t j = 0; j < numOfImage; ++j) {		// num of iamge
			Z_r[i][j] = pixelRaw[j][i].r;
			Z_g[i][j] = pixelRaw[j][i].g;
			Z_b[i][j] = pixelRaw[j][i].b;
		}

	}
	




	return 0;
}



