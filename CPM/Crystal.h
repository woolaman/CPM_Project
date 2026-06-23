#pragma once
#include <QVector>
#include "Histogram.h"

class Crystal
{
public:
    Crystal();
    Crystal(int ID);
    ~Crystal();

    int GetID() const;
    void SetID(int ID);

    int GetEntries();
    int GetEntries(qreal Emin, qreal Emax);

    void Fill(quint16 e);
    void CalRecEHist();

    void CalSlope();

    Histogram* GetADCHist();
    Histogram* GetRecEHist();

    void SetADCHist(Histogram* aHist);
    void SetRecEHist(Histogram* aHist);

    qreal GetSlope() const;
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