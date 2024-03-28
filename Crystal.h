#ifndef CRYSTAL_H
#define CRYSTAL_H

#include <QVector>
#include "Histogram.h"


class Crystal
{
public:
    Crystal();
    Crystal(int ID);
    ~Crystal();

    int GetID();
    void SetID(int ID);

    int GetEntries();

    void Fill(quint16 e);
    void CalRecEHist();

    Histogram* GetADCHist();
    Histogram* GetRecEHist();

    qreal GetSlope();
    void SetSlope(qreal x);

    qreal GetER();

    void Clear();


private:
    int m_ID;
    qreal m_slope;
    QVector<quint16> m_eList;
    Histogram* m_ADCHist;
    Histogram* m_recEHist;
};

#endif // CRYSTAL_H
