#include "ReinhardAlgo.hpp"

#include "Common.hpp"

#include <iostream>

cv::Mat HDRI::ReinhardAlgo::toneMap(const cv::Mat &inputRadiance) {

    const float epsilon = 0.00001f;
    const float a = 0.18f; // from paper

    // color space transform
    cv::Mat lumi(inputRadiance.size(), CV_32FC1);
    for (auto y = 0; y < inputRadiance.size().height; ++y) {
        for (auto x = 0; x < inputRadiance.size().width; ++x) {

            // L = 0.2126 * R + 0.7152 * G + 0.0722 * B;
            // lumi.at<double>(y, x) = 0.2126 * inputRadiance.at<cv::Vec3f>(y,
            // x)[2] + 0.7152 * inputRadiance.at<cv::Vec3f>(y, x)[1] + 0.0722 *
            // inputRadiance.at<cv::Vec3f>(y, x)[0];
            lumi.at<float>(y, x) =
                convertRGB(inputRadiance.at<cv::Vec3f>(y, x)[2],
                           inputRadiance.at<cv::Vec3f>(y, x)[1],
                           inputRadiance.at<cv::Vec3f>(y, x)[0]);
        }
    }

    double Lw_bar = 0.0;

    // loop over all values in the Mat
    for (auto y = 0; y < inputRadiance.size().height; ++y) {

        for (auto x = 0; x < inputRadiance.size().width; ++x) {

            Lw_bar += std::log(lumi.at<float>(y, x) + epsilon); // from paper
        }
    }

    std::cerr << "Lw_bar: " << Lw_bar << std::endl;

    const size_t N = inputRadiance.total();

    // Equation 1 in the paper is wrong. The division by N should be placed
    // before the summation, not outside the exponentiation.
    Lw_bar = std::exp(Lw_bar / N);

    float coeff = a / Lw_bar;

    float L_white = 1.7f; // test

    // compute Ld
    cv::Mat Ld(inputRadiance.size(), CV_32FC1);

    for (auto y = 0; y < inputRadiance.size().height; ++y) {
        for (auto x = 0; x < inputRadiance.size().width; ++x) {

            float L =
                coeff * lumi.at<float>(y, x); // Ld = (a / Lw_bar ) * (Lw(x,y))
            Ld.at<float>(y, x) =
                L * (1.0f + L / (L_white * L_white)) / (1.0f + L);

            // Ld.at<float>(y, x) = L;

            if (L > L_white) {
                Ld.at<float>(y, x) = 1;
            }
        }
    }

    cv::Mat outputImage(inputRadiance.size(), CV_8UC3);
    for (size_t idx = 0; idx < 3; ++idx) { // rgb

        for (auto y = 0; y < inputRadiance.size().height; ++y) {
            for (auto x = 0; x < inputRadiance.size().width; ++x) {

                outputImage.at<cv::Vec3b>(y, x)[idx] = cv::saturate_cast<uchar>(
                    inputRadiance.at<cv::Vec3f>(y, x)[idx] *
                    (Ld.at<float>(y, x) * 255.0 / lumi.at<float>(y, x)));

                // std::cerr << "out: " << Ld.at<float>(y, x) << std::endl;
            }
        }
    }

    return outputImage;
}
