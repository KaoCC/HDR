#ifndef _HDRI_TONEMAPALGO_HPP_
#define _HDRI_TONEMAPALGO_HPP_

#include <opencv2/core/mat.hpp>

namespace HDRI {

class ToneMapAlgo {

  public:
    virtual cv::Mat toneMap(const cv::Mat &inputRadiance) = 0;
    virtual ~ToneMapAlgo() = default;
};

} // namespace HDRI

#endif
