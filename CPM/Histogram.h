/**
 * @class Histogram class
 *
 * 基础工具类，直方图类。
 *
*/
#pragma once
#include <QPointF>
#include <QVector>

class Histogram
{
public:
    Histogram();
    Histogram(qreal aXmin, qreal aXmax, int nBins);
    ~Histogram();

    void Fill(qreal value);

    qreal GetBinWidth() const;
    qreal GetBinCenter(int iBin);
    qreal GetBinContent(int iBin);

    void SetBinContent(int iBin, qreal value);

    QVector<qreal> GetBinContents();
    void SetBinContents(QVector<qreal> aVec) { m_binContents = aVec; }

    void Smooth(int windowSize=5);
    void Smooth(int windowSize, int times);

    void SetCutValue(qreal value);

    void Add(Histogram* aHist);

    QPointF GetPeak();
    qreal GetResolution();
    qreal GetLeftValue() const;
    qreal GetRightValue() const;

    void Clear();

    Histogram* Clone();

    qreal GetMaxHeight();

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