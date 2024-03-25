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

    qreal GetBinContent(int iBin);
    QVector<qreal> GetBinContents();

    qreal GetBinCenter(int iBin);
    QVector<qreal> GetBinCenters();

    void SetBinContent(int iBin, qreal value);
    void SetBinContents(QVector<qreal>& vec);

    void Smooth(int windowSize=10);
    void Smooth(int windowSize, int times);

    void SetCutValue(qreal value);
    QPointF GetPeak();
    qreal GetResolution();

    void Add(Histogram& aHist);

    qreal GetLeftValue(){return m_leftValue;}
    qreal GetRightValue(){return m_rightValue;}

private:
    qreal m_xmin;
    qreal m_xmax;

    int m_nBins;
    qreal m_binWidth;

    // < m_cutValue, bin heiht = 0, using it in finding peak
    qreal m_cutValue;

    QVector<qreal> m_binContents;
    QVector<qreal> m_binCenters;

    qreal m_leftValue;
    qreal m_rightValue;
};

#endif // HISTOGRAM_H
