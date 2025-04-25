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


/**
 * @brief 计算能窗范围内的事例数。
 */
int Crystal::GetEntries(qreal Emin, qreal Emax)
{
    int counter = 0;
    for (int i = 0; i < m_eList.size(); ++i)
    {
        qreal recE = m_slope * m_eList[i];
        if(Emin<recE && recE<Emax)
        {
            counter++;
        }
    }
    return counter;
}


void Crystal::Fill(quint16 e)
{
    m_eList.append(e);
    m_ADCHist->Fill(e);
}

/**
 * @brief 晶体校正能谱过程。对ADC谱进行平滑、寻峰，计算ADC->keV转换系数，然后计算keV能谱
 */
void Crystal::CalRecEHist()
{
    auto h = m_ADCHist->Clone();
    h->SetCutValue(ADC_cutValue);
    h->Smooth(10, 2);

    qreal peakLoc = h->GetPeak().x();
    m_slope = peakE/peakLoc;

    for (int i = 0; i < m_eList.size(); ++i)
    {
        m_recEHist->Fill(m_slope * m_eList[i]);
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
    auto h = m_recEHist->Clone();
    h->Smooth(5);
    return h->GetResolution();
}


void Crystal::Clear()
{
    m_eList.clear();
    m_ADCHist->Clear();
    m_recEHist->Clear();
}

