#ifndef BLOCK_H
#define BLOCK_H

#include "Crystal.h"
#include "Parameters.h"

#include <opencv2/core.hpp>


class Block
{
public:
    Block();
    Block(int ID);
    ~Block();

    int GetID();
    void SetID(int ID);

    void Fill(quint16 x, quint16 y, quint16 e);

    void CalMap(int EW_min, int EW_max);

    void Segment(SegmentMethod method);
    void Segment1(); // SVD method
    void Segment2(); // find maximum value method

    void ManualAdjust(QPoint pos);

    void CalSegResult();
    void CalRecEHist();
    void CalSegFOM();

    void GenPositionLUT();
    void GenEnergyLUT();
    void GenUniformityLUT();

    Crystal* GetCrystal(int ID);
    Histogram* GetADCHist();
    Histogram* GetRecEHist();

    cv::Mat_<qreal> GetMap();
    cv::Mat_<cv::Vec2w> GetPeakTable();
    cv::Mat_<quint16> GetSegResult();
    cv::Mat_<qreal> GetSegMap();

    qreal GetER();
    qreal GetPVR();
    qreal GetFWHM();

    qreal CalFWHM(cv::Mat I);

    void Clear();


private: // private members
    int m_ID;

    QVector<quint16> m_xList;
    QVector<quint16> m_yList;
    QVector<quint16> m_eList;

    QVector<Crystal*> m_crystals;

    Histogram* m_ADCHist;
    Histogram* m_recEHist;

    cv::Mat_<qreal> m_I0;
    cv::Mat_<quint16> m_segr;
    cv::Mat_<quint8> m_edge;
    cv::Mat_<qreal> m_segMap;

    cv::Mat_<cv::Vec2w> m_pt;

    int m_num1;
    int m_num2;

    qreal m_PVR;
    qreal m_FWHM;


private: // private functions
    void CalGroupPar();
    QVector<int> FindPeaks(QVector<qreal> v, int nPeaks);

};

#endif // BLOCK_H
