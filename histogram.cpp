#include "Histogram.h"

#include "opencv2/core/mat.hpp"
#include "opencv2/imgproc.hpp"


Histogram::Histogram() { }


Histogram::Histogram(double xmin, double xmax, int nBins)
{   m_xmin = xmin;
    m_xmax = xmax;
    m_nBins = nBins;

    m_binWidth = (m_xmax-m_xmin)/m_nBins;
    m_binContents = QVector<double>(m_nBins, 0);
    m_binCenters = QVector<double>(m_nBins, 0);
    for (int i = 0; i < m_nBins; ++i)
    {
        m_binCenters[i] = i*m_binWidth + m_binWidth/2;
    }
}


Histogram::~Histogram() { }


template<typename T>
void Histogram::Fill(T& value)
{
    int idx = std::floor(value/m_binWidth);
    m_binContents[idx]++;
}


template<typename T>
void Histogram::Fill(QVector<T>& vec)
{
    for (const auto& var : vec)
    {
        int idx = std::floor(var/m_binWidth);
        m_binContents[idx]++;
    }
}


double Histogram::GetBinWidth()
{
    return m_binWidth;
}


double Histogram::GetBinContent(int iBin)
{
    return m_binContents[iBin];
}


QVector<double> Histogram::GetBinContents()
{
    return m_binContents;
}


double Histogram::GetBinCenter(int iBin)
{
    return m_binCenters[iBin];
}

QVector<double> Histogram::GetBinCenters()
{
    return m_binCenters;
}


void Histogram::Smooth(int windowSize=10)
{
	// 将 std::vector 转换为 cv::Mat
    QVector<int> data;
    cv::Mat dataMat(1, data.size(), CV_32F);
    for (size_t i = 0; i < data.size(); ++i)
    {
        dataMat.at<float>(0, i) = data[i];
    }

    // 使用 OpenCV 的平滑函数对数据进行平滑处理
    cv::Mat smoothedDataMat;
    cv::blur(dataMat, smoothedDataMat, cv::Size(windowSize, 1));

    // 将平滑后的数据转换为 std::vector
    QVector<float> smoothedData(smoothedDataMat.begin<float>(),
                                smoothedDataMat.end<float>());
    // smoothedData;
}


void Histogram::Smooth(int windowSize, int times)
{
    for (int i = 0; i < times; ++i)
    {
        Smooth(windowSize);
    }
}

