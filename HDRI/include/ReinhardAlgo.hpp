#ifndef _HDRI_REINHARDALGO_HPP_
#define _HDRI_REINHARDALGO_HPP_

#include "ToneMapAlgo.hpp"

namespace HDRI {

class ReinhardAlgo : public ToneMapAlgo {

  public:
    virtual cv::Mat toneMap(const cv::Mat &inputRadiance) override;
};

} // namespace HDRI

#endif
