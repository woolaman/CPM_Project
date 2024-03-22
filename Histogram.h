#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QPointF>
#include <QVector>


class Histogram
{
public:
    Histogram();
    Histogram(qreal xmin, qreal xmax, int nBins);
    ~Histogram();

    template<typename T> void Fill(T& value);
    template<typename T> void Fill(QVector<T>& vec);

    qreal GetBinWidth();

    qreal GetBinContent(int iBin);
    QVector<qreal> GetBinContents();

    qreal GetBinCenter(int iBin);
    QVector<qreal> GetBinCenters();

    void Smooth(int windowSize);
    void Smooth(int windowSize, int times);

    void SetCutValue(qreal value);
    QPointF GetPeak();
    qreal GetResolution();


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
