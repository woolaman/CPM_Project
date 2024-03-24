#include "Histogram.h"

#include "opencv2/core/mat.hpp"
#include "opencv2/imgproc.hpp"

#include <QtMath>


Histogram::Histogram() { }


Histogram::Histogram(qreal xmin, qreal xmax, int nBins)
{
    m_xmin = xmin;
    m_xmax = xmax;
    m_nBins = nBins;

    m_binWidth = (m_xmax-m_xmin)/m_nBins;
    m_binContents = QVector<qreal>(m_nBins, 0);
    m_binCenters = QVector<qreal>(m_nBins, 0);
    for (int i = 0; i < m_nBins; ++i)
    {
        m_binCenters[i] = m_xmin + i*m_binWidth + m_binWidth/2;
    }
}


Histogram::~Histogram() { }


void Histogram::Fill(int value)
{
    int idx = qFloor( (value-m_xmin)/m_binWidth );
    m_binContents[idx]++;
}


void Histogram::Fill(QVector<int> vec)
{
    for (auto var : vec)
    {
        int idx = qFloor( (var-m_xmin)/m_binWidth );
        m_binContents[idx]++;
    }
}


qreal Histogram::GetBinWidth()
{
    return m_binWidth;
}


qreal Histogram::GetBinContent(int iBin)
{
    return m_binContents[iBin];
}


QVector<qreal> Histogram::GetBinContents()
{
    return m_binContents;
}


qreal Histogram::GetBinCenter(int iBin)
{
    return m_binCenters[iBin];
}


QVector<qreal> Histogram::GetBinCenters()
{
    return m_binCenters;
}


void Histogram::SetBinContent(int iBin, qreal value)
{
    m_binContents[iBin] = value;
}


void Histogram::SetBinContents(QVector<qreal>& vec)
{
    for (int i = 0; i < m_nBins; ++i)
    {
        m_binContents[i] = vec[i];
    }
}


void Histogram::Smooth(int windowSize=10)
{
	// 将 std::vector 转换为 cv::Mat
    cv::Mat_<qreal> data0(1, m_nBins);
    for (int i = 0; i < m_nBins; ++i)
    {
        data0(0, i) = m_binContents[i];
    }

    // 使用 OpenCV 的平滑函数对数据进行平滑处理
    cv::Mat_<qreal> data1;
    cv::blur(data0, data1, cv::Size(windowSize, 1));

    // 将平滑后的数据返回给 m_binContents
    for (int i = 0; i < m_nBins; ++i)
    {
        m_binContents[i] = data1(0, i);
    }
}


void Histogram::Smooth(int windowSize, int times)
{
    for (int i = 0; i < times; ++i)
    {
        Smooth(windowSize);
    }
}


void Histogram::SetCutValue(qreal value)
{
    m_cutValue = value;
}


QPointF Histogram::GetPeak()
{
    int cutIdx = qRound((m_cutValue-m_xmin)/m_binWidth);
    int peakIdx = std::max_element(m_binContents.begin()+cutIdx,
                                   m_binContents.end()) - m_binContents.begin();
    qreal x = m_binCenters[peakIdx];
    qreal y = m_binContents[peakIdx];
    return QPointF(x, y);
}


qreal Histogram::GetResolution()
{
    int cutIdx = qRound((m_cutValue-m_xmin)/m_binWidth);
    int peakIdx = std::max_element(m_binContents.begin()+cutIdx,
                                   m_binContents.end()) - m_binContents.begin();
    qreal peak_x = m_binCenters[peakIdx];
    qreal peak_y = m_binContents[peakIdx];

    qreal halfMax = peak_y/2;
    int leftIdx = peakIdx;
    int rightIdx = peakIdx;

    while (leftIdx>0 && m_binContents[leftIdx]>halfMax)
    {
        leftIdx--;
    }

    while (rightIdx<m_nBins-1 && m_binContents[rightIdx]>halfMax)
    {
        rightIdx++;
    }

    qreal leftValue = m_binCenters[leftIdx];
    qreal rightValue = m_binCenters[rightIdx];
    qreal FWHM = rightValue - leftValue; // 半高宽
    return FWHM/peak_x;
}


void Histogram::Add(Histogram& aHist)
{
    for (int i = 0; i < m_nBins; ++i)
    {
        m_binContents[i] += aHist.GetBinContent(i);
    }
}


Histogram Histogram::operator+(const Histogram& b)
{
    Histogram c(this->m_xmin, this->m_xmax, this->m_nBins);
    for (int i = 0; i < this->m_nBins; ++i)
    {
        c.SetBinContent(i, this->m_binContents[i] + b.m_binContents[i]);
    }

    return c;
}


Histogram& Histogram::operator+=(const Histogram& b)
{
    for (int i = 0; i < m_nBins; ++i)
    {
        m_binContents[i] += b.m_binContents[i];
    }
    return *this;
}

