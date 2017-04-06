#ifndef _HDRI_RAWIMAGE_HPP_
#define _HDRI_RAWIMAGE_HPP_

#include <opencv2/photo.hpp>


namespace HDRI {




	class rawImage {

	public:
		rawImage()  =default;

		void load(const std::string fileName, double ss);

	private:

		
		cv::Mat mImageData;
		std::string mName;
		double mInvShutterSpeed;

	};



}






#endif
