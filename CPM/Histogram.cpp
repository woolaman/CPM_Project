/**
 * @details 直方图类。直方图类实现了平滑、寻峰、计算半高宽、计算分辨率等基础操作。
 * crystal、block等对象调用直方图类可以计算自己的参数，使得代码更加整洁清晰。
*/
#include "Histogram.h"

#include "opencv2/core/mat.hpp"
#include "opencv2/imgproc.hpp"

#include <QtMath>
#include <QDebug>

Histogram::Histogram() { }

Histogram::Histogram(qreal aXmin, qreal aXmax, int nBins)
{
    m_xmin = aXmin;
    m_xmax = aXmax;
    m_nBins = nBins;

    m_binWidth = (m_xmax-m_xmin)/m_nBins;
    m_binCenters = QVector<qreal>(m_nBins, 0);
    m_binContents = QVector<qreal>(m_nBins, 0);

    for (int i = 0; i < m_nBins; ++i)
    {
        m_binCenters[i] = m_xmin + i*m_binWidth + m_binWidth/2;
    }

    m_cutValue = 0;
    m_leftValue = 0;
    m_rightValue = 0;
}

Histogram::~Histogram() { }

void Histogram::Fill(qreal value)
{
    int idx = qFloor( (value-m_xmin)/m_binWidth );

    if(0<=idx && idx<m_nBins)
    {
        m_binContents[idx] += 1;
    }
}

qreal Histogram::GetBinWidth()
{
    return m_binWidth;
}

qreal Histogram::GetBinCenter(int iBin)
{
    return m_binCenters[iBin];
}

qreal Histogram::GetBinContent(int iBin)
{
    return m_binContents[iBin];
}

void Histogram::Smooth(int windowSize)
{
    // 将 QVector 转换为 cv::Mat
    cv::Mat_<qreal> data0(1, m_nBins);
    for (int i = 0; i < m_nBins; ++i)
    {
        data0(0, i) = m_binContents[i];
    }

    // 使用 OpenCV 的平滑函数对数据进行平滑处理
    cv::Mat_<qreal> data1;
    cv::blur(data0, data1, cv::Size(windowSize, 1));
    //cv::GaussianBlur(data0, data1, cv::Size(windowSize, 1), 0, 0); // broken

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

void Histogram::Add(Histogram* aHist)
{
    for (int i = 0; i < m_nBins; ++i)
    {
        m_binContents[i] += aHist->GetBinContent(i);
    }
}

QPointF Histogram::GetPeak()
{
    //int cutIdx = qRound((m_cutValue-m_xmin)/m_binWidth);
    //int peakIdx = std::max_element( m_binContents.begin() + cutIdx,
    //                               m_binContents.end() ) - m_binContents.begin();
    //qreal x = m_binCenters[peakIdx];
    //qreal y = m_binContents[peakIdx];
    //return QPointF(x, y);

    // 插值 //method: INTER_CUBIC, INTER_LINEAR
    std::vector<qreal> vx;
    std::vector<qreal> vy;

    int enlargeN = 10;
    int targetLength = m_nBins * enlargeN; // 目标长度

    cv::Mat_<qreal> bins(1, m_nBins, m_binCenters.data());
    cv::Mat resizedBins;
    cv::resize(bins, resizedBins, cv::Size(targetLength, 1), 0, 0, cv::INTER_LINEAR); 
    resizedBins.copyTo(vx);

    cv::Mat_<qreal> contents(1, m_nBins, m_binContents.data());
    cv::Mat resizedContents;
    cv::resize(contents, resizedContents, cv::Size(targetLength, 1), 0, 0, cv::INTER_CUBIC);
    resizedContents.copyTo(vy);

    int cutIdx = qRound((m_cutValue - m_xmin) / (1.0*m_binWidth / enlargeN));

    int peakIdx = std::max_element(vy.begin() + cutIdx, vy.end()) - vy.begin();
    qreal peak_x = vx[peakIdx];
    qreal peak_y = vy[peakIdx];

    return QPointF(peak_x, peak_y);
}

qreal Histogram::GetResolution()
{
    //QVector<qreal> vx = m_binCenters;
    //QVector<qreal> vy = m_binContents;
    //int cutIdx = qRound((m_cutValue - m_xmin) / m_binWidth);

    // 插值
    //*************************************************************************
    std::vector<qreal> vx;
    std::vector<qreal> vy;

    int enlargeN = 10;
    int targetLength = m_nBins * enlargeN; // 目标长度

    cv::Mat_<qreal> bins(1, m_nBins, m_binCenters.data());
    cv::Mat resizedBins;
    cv::resize(bins, resizedBins, cv::Size(targetLength, 1), 0, 0, cv::INTER_LINEAR); //INTER_CUBIC, INTER_LINEAR
    resizedBins.copyTo(vx);

    cv::Mat_<qreal> contents(1, m_nBins, m_binContents.data());
    cv::Mat resizedContents;
    cv::resize(contents, resizedContents, cv::Size(targetLength, 1), 0, 0, cv::INTER_CUBIC);
    resizedContents.copyTo(vy);
    
    int cutIdx = qRound((m_cutValue - m_xmin) / (1.0*m_binWidth / enlargeN));
    //**************************************************************************/

    int peakIdx = std::max_element(vy.begin() + cutIdx, vy.end()) - vy.begin();
    qreal peak_x = vx[peakIdx];
    qreal peak_y = vy[peakIdx];

    qreal halfMax = 0.5*peak_y;
    int leftIdx = peakIdx;
    int rightIdx = peakIdx;

    while (leftIdx>0 && vy[leftIdx]>halfMax)
    {
        leftIdx--;
    }

    while (rightIdx<vy.size()-1 && vy[rightIdx]>halfMax)
    {
        rightIdx++;
    }

    m_leftValue = vx[leftIdx];
    m_rightValue = vx[rightIdx];
    qreal FWHM = m_rightValue - m_leftValue; // 半高宽

    if(0 == peak_x)
    {
        qDebug() << "peak x is 0, please check the crystal's data.";
        return 0;
    }

    return FWHM/peak_x;
}

qreal Histogram::GetLeftValue()
{
    return m_leftValue;
}

qreal Histogram::GetRightValue()
{
    return m_rightValue;
}

void Histogram::Clear()
{
    for (auto& var : m_binContents)
    {
        var = 0;
    }
}

void Histogram::SetBinContent(int iBin, qreal value)
{
    m_binContents[iBin] = value;
}

Histogram* Histogram::Clone()
{
    Histogram* h = new Histogram(m_xmin, m_xmax, m_nBins);
    for (int iBin = 0; iBin < m_nBins; ++iBin)
    {
        h->SetBinContent(iBin, m_binContents[iBin]);
    }
    return h;
}

QVector<qreal> Histogram::GetBinContents()
{
    return m_binContents;
}