#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QPointF>
#include <QVector>


class Histogram
{
public:
    Histogram();
    Histogram(qreal aXmin, qreal aXmax, int nBins);
    ~Histogram();

    void Fill(qreal value);

    qreal GetBinWidth();
    qreal GetBinCenter(int iBin);
    qreal GetBinContent(int iBin);

    void Smooth(int windowSize=10);
    void Smooth(int windowSize, int times);

    void SetCutValue(qreal value);

    void Add(Histogram* aHist);

    QPointF GetPeak();
    qreal GetResolution();
    qreal GetLeftValue();
    qreal GetRightValue();

    void Clear();


private:
    int m_nBins;
    qreal m_xmin;
    qreal m_xmax;
    qreal m_binWidth;

    QVector<qreal> m_binCenters;
    QVector<qreal> m_binContents;

    qreal m_cutValue;

    qreal m_leftValue;
    qreal m_rightValue;
};

#endif // HISTOGRAM_H
