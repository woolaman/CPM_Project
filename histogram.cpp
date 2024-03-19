#include "histogram.h"


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

    QVector<double> smoothedData(m_nBins, 0);

    for (int i = 0; i < m_nBins; ++i)
    {
        double sum = 0.0;
        int counter = 0;

        // 计算窗口范围内的数据点的和
        int low = std::max(0, i - windowSize/2 + 1);
        int up = std::min(i+windowSize/2, m_nBins);

        double mean = std::accumulate(m_binContents.begin()+low, m_binContents.begin()+up, 0.0)/(up-low);

        // 计算平均值并添加到平滑数据中
        smoothedData[i] = mean;
    }

    m_binContents =  smoothedData;
}


void Histogram::Smooth(int windowSize, int times)
{
    for (int i = 0; i < times; ++i)
    {
        Smooth(windowSize);
    }
}

