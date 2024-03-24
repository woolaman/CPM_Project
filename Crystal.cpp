#include "Crystal.h"

Crystal::Crystal()
{
    // m_peakE = 511; // keV
}


Crystal::Crystal(int crystalID)
{
    m_ID = crystalID;
}

Crystal::Crystal(int rowID, int colID)
{
    m_rowID = rowID;
    m_colID = colID;
}


Crystal::~Crystal() { }


int Crystal::GetID()
{
    return m_ID;
}


int Crystal::GetRowID()
{
    return m_rowID;
}


int Crystal::GetColID()
{
    return m_colID;
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


void Crystal::Fill(quint16 x, quint16 y, quint16 e)
{
    m_xList.append(x);
    m_yList.append(y);
    m_eList.append(e);
}


void Crystal::Fill(QVector<quint16> data)
{
    for (int i = 0; i < data.size(); ++i)
    {
        if(i%3==0)
        {
            m_xList.append(data[i]);
        }

        if(i%3==1)
        {
            m_yList.append(data[i]);
        }

        if(i%3==2)
        {
            m_eList.append(data[i]);
        }
    }
}


void Crystal::SetPeakEnergy(qreal e)
{
    m_peakE = e;
}


void Crystal::CalADCHist()
{
    m_ADCHist = Histogram(ADC_min, ADC_max, ADC_nBins);
    for (auto var : m_eList)
    {
        m_ADCHist.Fill(var);
    }

    m_ADCHist.SetCutValue(ADC_cutValue);

    qreal peakLoc = m_ADCHist.GetPeak().x();
    m_slope = m_peakE/peakLoc;
}


void Crystal::CalRecEHist()
{
    m_recEHist = Histogram(recE_min, recE_max, recE_nBins);
    for (auto var : m_eList)
    {
        qreal recE = m_slope * var;
        m_recEHist.Fill(recE);
    }
}


qreal Crystal::GetSlope()
{
    return m_slope;
}


qreal Crystal::GetER()
{
    return m_recEHist.GetResolution();
}


Histogram Crystal::GetRecEHist()
{
    return m_recEHist;
}


Histogram Crystal::GetADCHist()
{

    return m_ADCHist;
}

