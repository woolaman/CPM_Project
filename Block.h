#ifndef BLOCK_H
#define BLOCK_H

#include <opencv2/core.hpp>

#include "Crystal.h"
#include "Parameters.h"


class Block
{
public:
    Block();
    ~Block();

    int GetID();
    void SetID(int ID);

    void Append(Crystal aCrystal);

    qreal GetER();

    void Fill(quint16 x, quint16 y, quint16 e);

    void SaveToFile(QString fName);

    Histogram GetADCHist(){return m_ADCHist;}
    Histogram GetRecEHist(){return m_recEHist;}

    cv::Mat_<qreal> GetMap(){return m_I0;}

    void CalMap(qreal EW_min, qreal EW_max);

    void SetMap(cv::Mat_<qreal> I);

    void Segment(QString method);
    void Segment1(); // SVD method
    void Segment2(); // find maximum value method

    cv::Mat_<cv::Vec2w> GetPeakTable();
    cv::Mat_<quint16> GetSegResult();

    void GenPositionLUT();
    void GenEnergyLUT();
    void GenUniformityLUT();

    Crystal GetCrystal(int ID);

    void ManualAdjust(QPoint pos);

    void GenSegResult();
    cv::Mat_<qreal> GetSegMap(){return m_segMap;}

    void CalSegFOM();
    qreal GetIR(){return m_IR;}
    qreal GetRMS(){return m_RMS;}


private: // private members
    QVector<quint16> m_xList;
    QVector<quint16> m_yList;
    QVector<quint16> m_eList;

    int m_BKID;
    int m_crystalNum;
    QVector<Crystal> m_crystals;

    Histogram m_ADCHist;
    Histogram m_recEHist;

    cv::Mat_<qreal> m_I0;
    cv::Mat_<cv::Vec2w> m_pt;
    cv::Mat_<quint16> m_segr;
    cv::Mat_<qreal> m_segMap;

    cv::Mat_<quint8> m_edge;

    int m_num1;
    int m_num2;

    qreal m_IR;
    qreal m_RMS;


private: // private functions
    void CalRecEHist();
    void CalADCHist();

    QVector<int> FindPeaks(QVector<qreal> v, int nPeaks);
};

#endif // BLOCK_H
