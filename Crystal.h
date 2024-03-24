#ifndef CRYSTAL_H
#define CRYSTAL_H

#include <QVector>
#include "Histogram.h"
#include "Parameters.h"

class Crystal
{
public:
    Crystal();
    Crystal(int crystalID);
    Crystal(int rowID, int colID);
    ~Crystal();

    int GetID();
    int GetRowID();
    int GetColID();

    void SetID(int ID);
    void SetID(int rowID, int colID);

    void Fill(quint16 x, quint16 y, quint16 e);
    void Fill(QVector<quint16> data);

    void SetPeakEnergy(qreal e);
    qreal GetSlope();
    qreal GetER();

    Histogram GetADCHist();
    Histogram GetRecEHist();


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
    qreal m_peakE;

    Histogram m_ADCHist;
    Histogram m_recEHist;

    void CalADCHist();
    void CalRecEHist();

    qreal m_slope;
};

#endif // CRYSTAL_H
