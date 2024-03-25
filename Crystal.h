#ifndef CRYSTAL_H
#define CRYSTAL_H

#include <QVector>
#include "Histogram.h"


class Crystal
{
public:
    Crystal();
    Crystal(int crystalID);
    Crystal(int rowID, int colID);
    ~Crystal();

    int GetID();
    void SetID(int ID);
    void SetID(int rowID, int colID);

    int GetRowID();
    int GetColID();
    void SetRowID(int ID);
    void SetColID(int ID);

    void Fill(quint16 x, quint16 y, quint16 e);

    void CalRecEHist();
    Histogram GetADCHist();
    Histogram GetRecEHist();
    qreal GetSlope();
    qreal GetER();

    void SetSlope(qreal x) {m_slope = x;}


private:
    qreal m_x; // coordinate
    qreal m_y;

    int m_ID;
    int m_rowID; // matrix index
    int m_colID;

    int m_nEvts;
    QVector<quint16> m_xList;
    QVector<quint16> m_yList;
    QVector<quint16> m_eList;

    qreal m_ER;
    qreal m_peakE = 511;

    Histogram m_ADCHist;
    Histogram m_recEHist;

    qreal m_slope;
};

#endif // CRYSTAL_H
