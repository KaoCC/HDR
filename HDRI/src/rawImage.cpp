#include "rawImage.hpp"
#include <opencv2/imgcodecs.hpp>

#include <iostream>

namespace HDRI {

	void RawImage::load(const std::string fileName, double ss) {

		mImageData = cv::imread(fileName);
		mInvShutterSpeed = ss;
		mName = fileName;

		if (mImageData.empty()) {
			std::cerr << "Fail to load: " + fileName << std::endl;
		} else {
			std::cerr << "Loaded: " << fileName << std::endl;
		}

	}


}
