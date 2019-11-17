
#include "Common.hpp"

#include "WeightFunction.hpp"
#include "rawImage.hpp"

#include <array>

#include <string>

#include <fstream>
#include <sstream>

#include <iostream>

void loadRawImages(const std::string &basePath, const std::string &fileName,
                   std::vector<HDRI::RawImage> &images) {

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

            std::cout << "file: " << imageFileName
                      << " shutter: " << invShutterSpeed << std::endl;

            // read image
            images[currentImageIndex++].load(basePath + imageFileName,
                                             invShutterSpeed);

            if (currentImageIndex == numOfImages) {
                break;
            }
        }
    }
}

cv::Mat constructRadiance(const std::vector<HDRI::RawImage> &imageFiles,
                          const std::array<cv::Mat, 3> &gCurves,
                          HDRI::WeightFunction &dwf,
                          const std::vector<double> &expo) {

    // Size ?
    int width = imageFiles[0].getWidth();
    int height = imageFiles[0].getHeight();

    // radiance ?
    cv::Mat hdrImg(height, width, CV_32FC3);

    for (auto idx = 0; idx < gCurves.size(); ++idx) { // r, g, b

        for (auto y = 0; y < height; ++y) {
            for (auto x = 0; x < width; ++x) {

                double weightedSum = 0.0;
                double result = 0.0;

                // loop over all imgs
                for (size_t k = 0; k < imageFiles.size(); ++k) {

                    int pixelColor = static_cast<int>(
                        imageFiles[k].getImageData().at<cv::Vec3b>(y, x)[idx]);
                    double w = dwf.getWeight(pixelColor);

                    result += static_cast<double>(
                        w * (gCurves[idx].at<double>(pixelColor, 0) -
                             std::log(expo[k])));
                    weightedSum += w;
                }

                // Be careful !
                if (weightedSum < std::numeric_limits<double>::epsilon() &&
                    weightedSum >
                        -std::numeric_limits<double>::epsilon()) { // near 0.0
                    hdrImg.at<cv::Vec3f>(y, x)[idx] = 0;
                } else {
                    hdrImg.at<cv::Vec3f>(y, x)[idx] =
                        std::exp(result / weightedSum);
                }
            }
        }
    }

    return hdrImg;
}

float convertRGB(float r, float g, float b) {
    return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}
