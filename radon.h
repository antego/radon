#ifndef RADON_H
#define RADON_H

#include <opencv2/core/core.hpp>

class Radon
{
public:
    static void radonSinc(cv::Mat& origin);
    static cv::Mat iradon(cv::Mat& sinogram, const std::vector<float>& angles);
};

#endif // RADON_H
