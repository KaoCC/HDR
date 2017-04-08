#ifndef _HDRI_RAWIMAGE_HPP_
#define _HDRI_RAWIMAGE_HPP_

#include <opencv2/photo.hpp>


namespace HDRI {




	class RawImage {

	public:
		RawImage()  =default;

		void load(const std::string fileName, double ss);

		size_t getTotalSize() const;

		size_t getWidth() const;
		size_t getHeight() const;

		const cv::Mat& getImageData() const;

	private:

		
		cv::Mat mImageData;
		std::string mName;
		double mInvShutterSpeed;

	};



}






#endif
