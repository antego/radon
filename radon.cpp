#include "radon.h"

using namespace std;

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

void Radon::radonSinc(cv::Mat& origin)
{
    //todo: not only square images
    int M = origin.cols;
    int N = origin.rows;
    int K = M;
    float dX = 1;
    float dY = 1;
    float xMin = -(M - 1) / 2 * dX;
    float yMin = -(N - 1) / 2 * dY;
    int R = 2 * M - 1;
    cv::Mat result = cv::Mat::zeros(R, 180, CV_8UC1);


    std::vector <float> angles;
    float pi = atan(1) * 4;
    for (int i = 0; i < 180; i++)
    {
        angles.push_back((float)i / 180 * pi);
    }

    for (int t = 0; t < angles.size(); t++)
    {
        float cosTheta = std::cos(angles[t]);
        float sinTheta = std::sin(angles[t]);
        float rhoOffset = xMin * (cosTheta + sinTheta);
        if (sinTheta > 1 / std::sqrt(2))
        {
            float alpha = - cosTheta / sinTheta;
            for (int r = 0; r < R; r++)
            {
                float beta = (rho(r, R) - rhoOffset) / (dX * sinTheta);
                int sum = 0;
                for (int m = 0; m < M; m++)
                {
                   int n = std::round(alpha * m + beta);
                   if (n >= 0 && n < N)
                   {
                       sum += origin.at<unsigned char>(m,n);
                   }
                }
                result.at<unsigned char>(r, t) =(unsigned char) dX*sum/std::abs(sinTheta) / (M + N);
            }
        }
        else
        {
            float alpha = - sinTheta / cosTheta;
            for (int r = 0; r < R; r++)
            {
                float beta = (rho(r, R) - rhoOffset) / (dX * cosTheta);
                int sum = 0;
                for (int m = 0; m < M; m++)
                {
                   int n = std::round(alpha * m + beta);
                   if (n >= 0 && n < N)
                   {
                       sum += origin.at<unsigned char>(n,m);
                   }
                }
                result.at<unsigned char>(r, t) =(unsigned char) dX*sum / std::abs(cosTheta) / (M + N);
            }
        }
    }
    cv::namedWindow("Display window", CV_WINDOW_NORMAL);
    cv::imshow("Display window", result);
    cv::waitKey();
    iradon(result);
}

void Radon::iradon(cv::Mat& sinogram)
{
    //not only square images
    int M = sinogram.rows / 2;
    int T = sinogram.cols;
    float dX = 1;
    float dTheta = 1;
    float dRho = dX / sqrt(2);
    int R = 2 * M - 1;
    float rhoMin = - (R - 1) / 2 * dRho;
    float xMin = -(M - 1) / 2 * dX;
    float pi = atan(1) * 4;


    float cosTheta[T];
    float sinTheta[T];
    cv::Mat result = cv::Mat::zeros(M, M, CV_8UC1);

    float rhoOffset = rhoMin / dRho;
    for (int t = 0; t < T; t++)
    {
        float theta = t * dTheta / 180 * pi;
        cosTheta[t] = cos(theta);
        sinTheta[t] = sin(theta);
    }
    float xc[M][T];
    float ys[M][T];
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
                int rl = round(rm);
                float w = rm - rl;
                sum = sum + (1-w)*sinogram.at<unsigned char>(rl, t) + w*sinogram.at<unsigned char>(rl+1, t);
            }
            //todo: comprehended fraction
            result.at<unsigned char>(m, n) = sum * dTheta / 100;
        }
    }
    cv::namedWindow("iradon");
    cv::imshow("iradon", result);
    cv::waitKey();
}
