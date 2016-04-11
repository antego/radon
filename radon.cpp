#include "radon.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QDebug>

const float PI=3.14159265358979f;

float p(int k)
{
    int p_min = 0;
    float dP = 1;
    return p_min + k * dP;
}

float rho(int r, int R)
{
    float dX = 1;
    float dRho = dX / std::sqrt(2);
    float rhoMin = - (R - 1) / 2 * dRho;

    return rhoMin + r * dRho;
}

float rnd(float number)
{
    return number < 0.0f ? ceil(number - 0.5f) : floor(number + 0.5f);
}

cv::Mat Radon::radonSinc(cv::Mat& origin, const std::vector<float>& angles)
{
    int M = origin.rows;
    int N = origin.cols;
    int K = M;
    float dX = 1;
    float dY = 1;
    float xMin = -(M - 1) / 2 * dX;
    float yMin = -(N - 1) / 2 * dY;
    int R = 2 * std::max(M, N) - 1;

    cv::Mat result = cv::Mat::zeros(R, angles.size(), CV_8UC1);
    unsigned char maxValue = 1;
    for (int t = 0; t < angles.size(); t++)
    {
        float cosTheta = std::cos(angles[t]);
        float sinTheta = std::sin(angles[t]);
        float rhoOffset = xMin * cosTheta + yMin * sinTheta;
        if (sinTheta > 1 / std::sqrt(2))
        {
            float alpha = - cosTheta / sinTheta;
            for (int r = 0; r < R; r++)
            {
                float beta = (rho(r, R) - rhoOffset) / (dX * sinTheta);
                int sum = 0;
                for (int m = 0; m < M; m++)
                {
                   int n = rnd(alpha * m + beta);
                   if (n >= 0 && n < N)
                   {
                       sum += origin.at<unsigned char>(m,n);
                   }
                }
                unsigned char val = dX * sum / std::abs(sinTheta) / std::sqrt(M * M + N * N);
                if (val > maxValue) {
                    maxValue = val;
                }
                result.at<unsigned char>(r, t) = val;
            }
        }
        else
        {
            float alpha = - sinTheta / cosTheta;
            for (int r = 0; r < R; r++)
            {
                float beta = (rho(r, R) - rhoOffset) / (dX * cosTheta);
                int sum = 0;
                for (int n = 0; n < N; n++)
                {
                   int m = rnd(alpha * n + beta);
                   if (m >= 0 && m < M)
                   {
                       sum += origin.at<unsigned char>(m,n);
                   }
                }
                unsigned char val = dX * sum / std::abs(cosTheta) / std::sqrt(M * M + N * N);
                if (val > maxValue) {
                    maxValue = val;
                }
                result.at<unsigned char>(r, t) = val;
            }
        }
    }
    result *= 255.0f / maxValue;
    return result;
}

cv::Mat Radon::iradon(cv::Mat& sinogram, const std::vector<float>& angles)
{
    int M = sinogram.rows / 2;
    int T = angles.size();
    float dX = 1;
    float dTheta = 1;
    float dRho = dX / sqrt(2);
    int R = sinogram.rows;
    float rhoMin = - (R - 1) / 2 * dRho;
    float xMin = -(M - 1) / 2 * dX;


    std::vector<float> cosTheta(T);
    std::vector<float> sinTheta(T);
    cv::Mat result = cv::Mat::zeros(M, M, CV_8UC1);

    std::vector <float> anglesRad;
    for (int i = 0; i < angles.size(); i++)
    {
        anglesRad.push_back(angles[i] / 180 * PI);
    }

    float rhoOffset = rhoMin / dRho;
    for (int t = 0; t < T; t++)
    {
        float theta = anglesRad[t];
        cosTheta[t] = std::cos(theta);
        sinTheta[t] = std::sin(theta);
    }

    float** xc = new float*[M];
    for(int i = 0; i < M; ++i)
        xc[i] = new float[T];

    float** ys = new float*[M];
    for(int i = 0; i < M; ++i)
        ys[i] = new float[T];

    for (int m = 0; m < M; m++)
    {
        float xRel = (xMin + m * dX) / dRho;
        for (int t = 0; t < T; t++)
        {
            xc[m][t] = xRel * cosTheta[t];
            ys[m][t] = xRel * sinTheta[t] - rhoOffset;
        }
    }
    for (int m = 0; m < M; m++)
    {
        for (int n = 0; n < M; n++)
        {
            int sum = 0;
            for (int t = 0; t < T; t++)
            {
                float rm = xc[m][t] + ys[n][t];
                int rl = rm;
                if (rl < R - 1 && rl >= 0) {
                    float w = rm - rl;
                    sum += (1 - w) * sinogram.at<unsigned char>(rl, t)
                                + w * sinogram.at<unsigned char>(rl + 1, t);
                }
            }
            sum = sum * dTheta / T;
            if (sum > 255)
                qDebug() << "bigger than 255!";
            if (sum < 0)
                qDebug() << "less than 0!" << sum;
            result.at<unsigned char>(m, n) = rnd(sum);
        }
    }

    for(int i = 0; i < M; ++i) {
        delete [] xc[i];
    }
    delete [] xc;

    for(int i = 0; i < M; ++i) {
        delete [] ys[i];
    }
    delete [] ys;

    return result;
}
