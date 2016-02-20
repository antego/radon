#include "capturer.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"


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
    if (!camera.isOpened()) {
        handleError("Can't initialize a camera");
        return;
    }
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

void Capturer::handleError(const QString& caption)
{
    emit error(caption);
    emit cameraStopped();
    emit finished();
}

