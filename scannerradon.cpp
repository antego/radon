#include "scannerradon.h"
#include "radon.h"

#include <opencv2/highgui/highgui.hpp>
#include <QDir>
#include <QDebug>
#include <stdexcpt.h>


ScannerRadon::ScannerRadon(int dTheta, int zScale, std::vector<float> angles, QFileInfoList fileList) :
    dTheta(dTheta),
    zScale(zScale),
    angles(angles),
    fileList(fileList)
{

}

void ScannerRadon::scan()
{
    try
    {
        QList<cv::Mat> unfaces;
        for (int i = 0; i < fileList.size(); i++)
        {
            cv::Mat image = cv::imread(fileList[i].absoluteFilePath().toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
            if (!image.data)
                throw QString("Can't read image \""  + fileList[i].fileName() + "\"");
            cv::Mat sinogram = Radon::radonSinc(image, angles);

            if (i == 0)
            {
                for (int j = 0; j < angles.size(); j++)
                    unfaces.push_back(cv::Mat::zeros(sinogram.cols, sinogram.rows, CV_8UC1));
                emit setStepsCount(unfaces.size());
            }

            for (int j = 0; j < sinogram.cols; j++)
            {
                cv::Mat unface = unfaces[j];
                for (int k = 0; k < sinogram.rows; k++)
                    unface.at<unsigned char>(j, k) = sinogram.at<unsigned char>(k, j);
            }
        }

        for (int i = 0; i < unfaces.size(); i++)
        {
            cv::Mat unface = unfaces[i];
            cv::imwrite(fileList[0].absolutePath().toStdString() + "/unface" + QString::number(i).toStdString() + ".bmp", unface);
            emit setCurrentCount(i + 1);
            qDebug() << "writing image " << i;
        }
    }
    catch (const QString& errorStr)
    {
        emit error(errorStr);
    }
    emit finished();
}

ScannerRadon::~ScannerRadon()
{

}
