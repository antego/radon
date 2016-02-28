#ifndef RADON_H
#define RADON_H
#include <opencv2/opencv.hpp>

class Radon
{
public:
    static void radon(cv::Mat& origin, std::vector<float> viewAngles);
    static void radonSinc(cv::Mat& origin);
};

#endif // RADON_H
