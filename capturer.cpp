#include "capturer.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QThread>
#include <stdexcept>

Capturer::Capturer(int camId, int frameWidth, int frameHeight) :
    camId(camId),
    frameWidth(frameWidth),
    frameHeight(frameHeight),
    shotRequested(false)
{

}

Capturer::~Capturer()
{

}

void Capturer::process()
{
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
            if (shotRequested)
            {
                saveFrame(frame);
                shotRequested = false;
            }
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

void Capturer::requestShot(QString pictureName)
{
    this->pictureName = pictureName;
    shotRequested = true;
}

void Capturer::saveFrame(const cv::Mat& frame)
{
    try
    {
        cv::imwrite(pictureName.toStdString(), frame);
        emit shotTaken();
    }
    catch (std::runtime_error& ex)
    {
        emit error((QString)"Error while saving picture\n" + ex.what());
    }
}
