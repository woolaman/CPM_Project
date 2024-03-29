﻿#include "Histogram.h"

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
    m_binContents = QVector<qreal>(m_nBins, 0);
    m_binCenters = QVector<qreal>(m_nBins, 0);
    for (int i = 0; i < m_nBins; ++i)
    {
        m_binCenters[i] = m_xmin + i*m_binWidth + m_binWidth/2;
    }
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
    int cutIdx = qRound((m_cutValue-m_xmin)/m_binWidth);
    int peakIdx = std::max_element( m_binContents.begin() + cutIdx,
                                   m_binContents.end() ) - m_binContents.begin();
    qreal x = m_binCenters[peakIdx];
    qreal y = m_binContents[peakIdx];
    return QPointF(x, y);
}


qreal Histogram::GetResolution()
{
    int cutIdx = qRound((m_cutValue-m_xmin)/m_binWidth);
    int peakIdx = std::max_element( m_binContents.begin() + cutIdx,
                                   m_binContents.end() ) - m_binContents.begin();
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

    m_leftValue = m_binCenters[leftIdx];
    m_rightValue = m_binCenters[rightIdx];
    qreal FWHM = m_rightValue - m_leftValue; // 半高宽

    /************************************************
    qDebug() << "peak x = " << peak_x << "peak y = " << peak_y;
    qDebug() << "half max = " << halfMax;
    qDebug() << "left x = " << m_leftValue <<
        "left height = " << m_binContents[leftIdx];
    qDebug() << "right x = " << m_rightValue <<
        "right height = " << m_binContents[rightIdx];
    qDebug() << "FWHM = " << FWHM;
    **************************************************/

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

