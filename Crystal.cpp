#include "Crystal.h"
#include "Parameters.h"

#include <QDebug>


Crystal::Crystal()
{
    m_ID = 0;
    m_slope = 0;
    m_ADCHist = new Histogram(ADC_min, ADC_max, ADC_nBins);
    m_recEHist = new Histogram(recE_min, recE_max, recE_nBins);
}


Crystal::Crystal(int ID)
{
    m_ID = ID;
    m_slope = 0;
    m_ADCHist = new Histogram(ADC_min, ADC_max, ADC_nBins);
    m_recEHist = new Histogram(recE_min, recE_max, recE_nBins);
}


Crystal::~Crystal()
{
    delete m_ADCHist;
    delete m_recEHist;
}


int Crystal::GetID()
{
    return m_ID;
}


void Crystal::SetID(int ID)
{
    m_ID = ID;
}


int Crystal::GetEntries()
{
    return m_eList.size();
}


void Crystal::Fill(quint16 e)
{
    m_eList.append(e);
    m_ADCHist->Fill(e);
}


void Crystal::CalRecEHist()
{
    m_ADCHist->SetCutValue(ADC_cutValue);

    m_ADCHist->Smooth(10, 2);

    qreal peakLoc = m_ADCHist->GetPeak().x();
    m_slope = peakE/peakLoc;
    for (auto var : m_eList)
    {
        m_recEHist->Fill(m_slope * var);
    }
}


Histogram* Crystal::GetRecEHist()
{
    return m_recEHist;
}


Histogram* Crystal::GetADCHist()
{

    return m_ADCHist;
}


qreal Crystal::GetSlope()
{
    return m_slope;
}


void Crystal::SetSlope(qreal x)
{
    m_slope = x;
}


qreal Crystal::GetER()
{
    m_recEHist->Smooth();
    return m_recEHist->GetResolution();
}


void Crystal::Clear()
{
    m_eList.clear();
    m_ADCHist->Clear();
    m_recEHist->Clear();
}

