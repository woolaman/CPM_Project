#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QPointF>
#include <QVector>


class Histogram
{
public:
    Histogram();
    Histogram(double xmin, double xmax, int nBins);
    ~Histogram();

    template<typename T>
    void Fill(T& value);

    template<typename T>
    void Fill(QVector<T>& vec);

    double GetBinWidth();

    double GetBinContent(int iBin);
    QVector<double> GetBinContents();

    double GetBinCenter(int iBin);
    QVector<double> GetBinCenters();

    QPointF GetPeak();
    double GetResolution();

    void Smooth(int windowSize);
    void Smooth(int windowSize, int times);
    // void Smooth(QString method, int par1, int par2);


private:
    double m_xmin;
    double m_xmax;

    int m_nBins;
    double m_binWidth;

    QVector<double> m_binContents;
    QVector<double> m_binCenters;
};

#endif // HISTOGRAM_H
