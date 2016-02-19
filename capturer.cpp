#include "capturer.h"
#include "opencv2/opencv.hpp"

#include <QThread>

Capturer::Capturer(int camId, int frameWidth, int frameHeight)
    : camId(camId)
    , frameWidth(frameWidth)
    , frameHeight(frameHeight)
{

}

Capturer::~Capturer()
{

}

void Capturer::process()
{
    //todo exception handling
    cv::VideoCapture camera(camId);
    camera.set(CV_CAP_PROP_FRAME_WIDTH, frameWidth);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT, frameHeight);

    const std::string windowName = "Camera " + camId;
    cv::namedWindow(windowName);
    while (!QThread::currentThread()->isInterruptionRequested())
    {
        cv::Mat frame;
        camera >> frame;
        cv::imshow(windowName, frame);
    }
    cv::destroyWindow(windowName);
    camera.release();
    emit cameraStopped();
    emit finished();
}

