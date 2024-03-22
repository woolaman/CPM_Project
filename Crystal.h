#ifndef CRYSTAL_H
#define CRYSTAL_H

#include <QVector>

#include "Histogram.h"

class Crystal
{
public:
    Crystal();
    ~Crystal();

    void SetADCHist(Histogram h);

    double GetER();

    int GetID();
    int GetRowID();
    int GetColID();

    void SetID(int ID);
    void SetID(int rowID, int colID);


private:
    float m_x; // coordinate
    float m_y;

    int m_rowID; // matrix index
    int m_colID;

    int m_ID;

    QVector<int> m_ADCs;
    QVector<float> m_recEs;

    int ADCPeakLoc;
    double recEPeakLoc;

    double m_ER;

    double m_peakE;

    Histogram m_ADCHist;
    Histogram m_eHist;

};

#endif // CRYSTAL_H
