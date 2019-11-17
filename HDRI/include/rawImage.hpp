#ifndef _HDRI_RAWIMAGE_HPP_
#define _HDRI_RAWIMAGE_HPP_

#include <opencv2/photo.hpp>

namespace HDRI {

class RawImage {

  public:
    RawImage() = default;

    void load(const std::string fileName, double ss);

    size_t getTotalSize() const;

    int getWidth() const;
    int getHeight() const;

    const cv::Mat &getImageData() const;

    double getExposure() const;

  private:
    cv::Mat mImageData;
    std::string mName;
    double expo;
};

} // namespace HDRI

#endif
