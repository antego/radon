#ifndef RADON_H
#define RADON_H
#include <opencv2/opencv.hpp>

class Radon
{
public:
    static void radonSinc(cv::Mat& origin);
    static void iradon(cv::Mat& sinogram);
};

#endif // RADON_H
