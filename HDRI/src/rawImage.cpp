#include "rawImage.hpp"
#include <opencv2/imgcodecs.hpp>

#include <iostream>

namespace HDRI {

	void RawImage::load(const std::string fileName, double ss) {

		mImageData = cv::imread(fileName);
		expo = 1/ss;
		mName = fileName;

		if (mImageData.empty()) {
			std::cerr << "Fail to load: " + fileName << std::endl;
			throw std::runtime_error("Fail to load" + fileName);
		} else {
			std::cerr << "Loaded: " << fileName << std::endl;
		}

		//// test code

		//cv::Mat shrinkMat;
		//cv::resize(mImageData, shrinkMat, cv::Size(mImageData.cols, mImageData.rows));

		//// check size
		//cv::resize(shrinkMat, shrinkMat, cv::Size(20 , 20));

		//mImageData = shrinkMat;

	}

	size_t RawImage::getTotalSize() const {
		return mImageData.total();
	}

	size_t RawImage::getWidth() const {
		return mImageData.size().width;
	}

	size_t RawImage::getHeight() const {
		return mImageData.size().height;
	}

	const cv::Mat & RawImage::getImageData() const {
		return mImageData;
	}

	double RawImage::getExposure() const {
		return expo;
	}


}
