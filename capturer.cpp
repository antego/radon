#include "capturer.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"


#include <QThread>

Capturer::Capturer(int camId, int frameWidth, int frameHeight) :
    camId(camId),
    frameWidth(frameWidth),
    frameHeight(frameHeight)
{

}

Capturer::~Capturer()
{

}

void Capturer::process()
{
    //todo exception handling
    try
    {
        cv::VideoCapture camera(camId);
        if (!camera.isOpened())
            throw QString("Can't open camera");
        camera.set(CV_CAP_PROP_FRAME_WIDTH, frameWidth);
        camera.set(CV_CAP_PROP_FRAME_HEIGHT, frameHeight);

        const std::string windowName = "Camera " + QString::number(camId).toStdString();
        cv::namedWindow(windowName);
        while (!QThread::currentThread()->isInterruptionRequested())
        {
            cv::Mat frame;
            bool read = camera.read(frame);
            if (!read)
                continue;
            cv::imshow(windowName, frame);
        }
        cv::destroyWindow(windowName);
        camera.release();
    }
    catch (const QString& errorStr)
    {
        emit error(errorStr);
    }
    emit cameraStopped();
    emit finished();
}
