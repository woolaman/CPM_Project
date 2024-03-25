#include "Crystal.h"
#include "Parameters.h"
#include <QDebug>

Crystal::Crystal()
{
    m_ADCHist = Histogram(ADC_min, ADC_max, ADC_nBins);
    m_recEHist = Histogram(recE_min, recE_max, recE_nBins);
}


Crystal::Crystal(int crystalID)
{
    m_ID = crystalID;
    m_ADCHist = Histogram(ADC_min, ADC_max, ADC_nBins);
    m_recEHist = Histogram(recE_min, recE_max, recE_nBins);
}

Crystal::Crystal(int rowID, int colID)
{
    m_rowID = rowID;
    m_colID = colID;
    m_ADCHist = Histogram(ADC_min, ADC_max, ADC_nBins);
    m_recEHist = Histogram(recE_min, recE_max, recE_nBins);
}


Crystal::~Crystal() { }


int Crystal::GetID()
{
    return m_ID;
}


void Crystal::SetID(int ID)
{
    m_ID = ID;
}


void Crystal::SetID(int rowID, int colID)
{
    m_rowID = rowID;
    m_colID = colID;
}


int Crystal::GetRowID()
{
    return m_rowID;
}


int Crystal::GetColID()
{
    return m_colID;
}


void Crystal::SetRowID(int ID)
{
    m_rowID = ID;
}


void Crystal::SetColID(int ID)
{
    m_colID = ID;
}

void Crystal::Fill(quint16 x, quint16 y, quint16 e)
{
    m_xList.append(x);
    m_yList.append(y);
    m_eList.append(e);
    m_ADCHist.Fill(e);
}


void Crystal::CalRecEHist()
{
    m_ADCHist.SetCutValue(ADC_cutValue);
    qreal peakLoc = m_ADCHist.GetPeak().x();
    m_slope = m_peakE/peakLoc;

    for (auto var : m_eList)
    {
        qreal recE = m_slope * var;
        m_recEHist.Fill(recE);
    }
    m_ER = m_recEHist.GetResolution();
}


qreal Crystal::GetSlope()
{
    return m_slope;
}


qreal Crystal::GetER()
{
    return m_ER;
}


Histogram Crystal::GetRecEHist()
{
    return m_recEHist;
}


Histogram Crystal::GetADCHist()
{

    return m_ADCHist;
}

