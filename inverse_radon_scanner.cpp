#include "inverse_radon_scanner.h"
#include "radon.h"

#include <opencv2/highgui/highgui.hpp>
#include <QDir>
#include <QDebug>
#include <stdexcpt.h>


InverseRadonScanner::InverseRadonScanner(int dK, int dRho, QFileInfoList fileList, std::vector<float> angles, shaftOrientation shaft) :
    dK(dK),
    dRho(dRho),
    fileList(fileList),
    shaft(shaft)
{
    if (fileList.size() != angles.size())
        throw new std::runtime_error("Set angles for all images");

    for (int i = 0; i < angles.size(); i++)
    {
        float angle = angles[i];
        for (int j = ++i; j < angles.size(); j++)
            if (angle == angles[j])
                throw new std::runtime_error("Repeating angles");
    }

    this->angles = angles;
}

void InverseRadonScanner::scan()
{
    try
    {
        QList<cv::Mat> sinograms;
        for (int i = 0; i < fileList.size(); i++)
        {
            cv::Mat image = cv::imread(fileList[i].absoluteFilePath().toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
            if (!image.data)
                throw QString("Can't read image \""  + fileList[i].fileName() + "\"");

            if (shaft == VERTICAL)
            {
                if (i == 0)
                    for (int j = 0; j < image.rows; j += dK)
                        sinograms.push_back(cv::Mat::zeros(image.cols / dRho, fileList.size(), CV_8UC1));

                for (int j = 0; j < sinograms.size(); j++)
                {
                    cv::Mat sinogram = sinograms[j];
                    for (int k = 0; k < image.cols / dRho; k++)
                        //maybe change order of column filling
                        sinogram.at<unsigned char>(k, i) = image.at<unsigned char>(j * dK, k * dRho);
                }
            }
            else
            {
                if (i == 0)
                    for (int j = 0; j < image.cols; j += dK)
                        sinograms.push_back(cv::Mat::zeros(image.rows / dRho, fileList.size(), CV_8UC1));

                for (int j = 0; j < sinograms.size(); j++)
                {
                    cv::Mat sinogram = sinograms[j];
                    for (int k = 0; k < image.rows / dRho; k++)
                        sinogram.at<unsigned char>(k, i) = image.at<unsigned char>(k * dRho, j * dK);
                }
            }
        }
        emit setStepsCount(sinograms.size());

        //stage were sinograms processed
        for (int j = 0; j < sinograms.size(); j++)
        {
            cv::Mat slice = Radon::inverseRadon(sinograms[j], angles);
            cv::imwrite(fileList[0].absolutePath().toStdString() + "/slice" + QString::number(j).toStdString() + ".bmp", slice);
            emit setCurrentCount(j + 1);
            qDebug() << "writing image " << j;
        }
    }
    catch (const QString& errorStr)
    {
        emit error(errorStr);
    }
    emit finished();
}

InverseRadonScanner::~InverseRadonScanner()
{

}

