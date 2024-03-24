#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QPointF>
#include <QVector>
#include "Parameters.h"

class Histogram
{
public:
    Histogram();
    Histogram(qreal xmin, qreal xmax, int nBins);
    ~Histogram();

    void Fill(int value);
    void Fill(QVector<int> vec);

    qreal GetBinWidth();

    qreal GetBinContent(int iBin);
    QVector<qreal> GetBinContents();

    qreal GetBinCenter(int iBin);
    QVector<qreal> GetBinCenters();

    void SetBinContent(int iBin, qreal value);
    void SetBinContents(QVector<qreal>& vec);

    void Smooth(int windowSize);
    void Smooth(int windowSize, int times);

    void SetCutValue(qreal value);
    QPointF GetPeak();
    qreal GetResolution();

    void Add(Histogram& aHist);
    // 重载相加操作符为成员函数
    Histogram operator+(const Histogram& b);
    Histogram& operator+=(const Histogram& b);

private:
    qreal m_xmin;
    qreal m_xmax;

    int m_nBins;
    qreal m_binWidth;

    // < m_cutValue, bin heiht = 0, using it in finding peak
    qreal m_cutValue;

    QVector<qreal> m_binContents;
    QVector<qreal> m_binCenters;
};

#endif // HISTOGRAM_H
