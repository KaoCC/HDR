



#include <vector>
#include <string>

#include <sstream>
#include <fstream> 

#include "rawImage.hpp"

#include <iostream>
#include <cstdint>




const std::string kDefaultBasePath = "../Resource/";
const std::string kDefaultFileList = "list.txt";



// (1/shutter_speed)
// Note: factor of two
static const double defaultShutterSpeed[] = {1/32, 1/16, 1/8, 1/4, 1/2, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};


static void loadRawImages(const std::string& basePath, const std::string& fileName, std::vector<HDRI::rawImage>& images) {

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

	std::vector<HDRI::rawImage> imageFiles;


	try {
		loadRawImages(kDefaultBasePath, kDefaultFileList, imageFiles);
	} catch (std::exception& e) {
		std::cerr << e.what() << '\n';
	}



	return 0;
}



