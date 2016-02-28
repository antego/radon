#include "radon.h"

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

void Radon::radon(cv::Mat& origin, std::vector<float> viewAngles)
{
    int M = origin.cols;
    int N = origin.rows;
    int K = M;
    float dX = 1;
    float dY = 1;
    float xMin = -(M - 1) / 2 * dX;
    float yMin = -(N - 1) / 2 * dY;
    cv::Mat result(K, viewAngles.size(), CV_8U);

    for (int k = 0; k < K; k++)
    {
        for (unsigned int h = 0; h < viewAngles.size(); h++)
        {
            float alpha = p(k) * dX / dY;
            float beta = (p(k) * xMin + viewAngles[h] - yMin) / dY;
            int sum = 0;
            for (int m = 0; m < M; m++)
            {
                int n = alpha * m + beta + 0.5;
                if (n >= 0 && n < N)
                {
                    sum += origin.at<unsigned char>(m,n);
                }
            }
            result.at<unsigned char>(k, h) = sum / (M + N);
        }
    }
    //cv::namedWindow("Display window");
    //cv::imshow("Display window", result);
}

void Radon::radonSinc(cv::Mat& origin)
{
    int M = origin.cols;
    int N = origin.rows;
    int K = M;
    float dX = 1;
    float dY = 1;
    float xMin = -(M - 1) / 2 * dX;
    float yMin = -(N - 1) / 2 * dY;
    int R = 2 * M - 1;
    cv::Mat result(R, 180, CV_8U);


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
                float mMin;
                float mMax;
                if (alpha > 0)
                {
                    mMin = std::max(0,(int)std::round(- (beta + 1/2) / alpha));
                    mMax = std::min(M - 1,(int)std::round( (N-1/2-beta) / alpha) );
                }
                else
                {
                    mMin = std::max(0,(int)std::round( (N-1/2-beta) / alpha) );
                    mMax = std::min(M-1,(int)std::round(  - (beta + 1/2) / alpha) );
                }
                int sum = 0;
                for (int m = mMin; m < mMax; m++)
                {
                    sum += origin.at<unsigned char>(m,std::round(alpha*m+beta));
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
                float mMin;
                float mMax;
                if (alpha > 0)
                {
                    mMin = std::max(0,(int)std::round( - (beta + 1/2) / alpha) );
                    mMax = std::min(M - 1,(int)std::round( (N-1/2-beta) / alpha) );
                }
                else
                {
                    mMin = std::max(0,(int)std::round( (N-1/2-beta) / alpha) );
                    mMax = std::min(M-1,(int)std::round(  - (beta + 1/2) / alpha) );
                }
                int sum = 0;
                for (int m = mMin; m < mMax; m++)
                {
                    sum += origin.at<unsigned char>(std::round(alpha*m+beta), m);
                }
                result.at<unsigned char>(r, t) =(unsigned char) dX*sum / std::abs(cosTheta) / (M + N);
            }
        }
    }
    cv::namedWindow("Display window");
    cv::imshow("Display window", result);
}

