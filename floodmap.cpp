#include "floodmap.h"

Floodmap::Floodmap()
{
    I0 = cv::Mat::zeros(nPixel, nPixel, CV_32FC1);
    m_segr = cv::Mat::zeros(nPixel, nPixel, CV_16UC1);
}


Floodmap::~Floodmap()
{

}


void Floodmap::SetCrysatlN(int n)
{
    m_nCrystal = n;
}



cv::Mat_<quint16> Floodmap::GetSegResult()
{
    // magic here


    return m_segr;
}


void Floodmap::SetMap(cv::Mat_<float> I)
{

    I0 = I;

}


void Floodmap::Fill(int x, int y)
{
    I0(y, x) += 1;
}










