#include "Crystal.h"
#include "Parameters.h"
#include <QDebug>

Crystal::Crystal()
{
    m_ID = 0;
    m_slope = 0;
    m_ADCHist = new Histogram(ADC_min, ADC_max, ADC_nBins);
    m_recEHist = new Histogram(recE_min, recE_max, recE_nBins);
    m_ADCHist->SetCutValue(ADC_cutValue);
}

Crystal::Crystal(int ID)
{
    m_ID = ID;
    m_slope = 0;
    m_ADCHist = new Histogram(ADC_min, ADC_max, ADC_nBins);
    m_recEHist = new Histogram(recE_min, recE_max, recE_nBins);
    m_ADCHist->SetCutValue(ADC_cutValue);
}

Crystal::~Crystal()
{
    delete m_ADCHist;
    delete m_recEHist;
}

int Crystal::GetID() const
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
    for (auto var : m_eList)
    {
        qreal recE = m_slope * var;
        if (Emin < recE && recE < Emax)
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

    for (auto var : m_eList)
    {
        m_recEHist->Fill(m_slope * var);
    }
}

void Crystal::CalSlope()
{
    auto h = m_ADCHist->Clone();
    h->SetCutValue(ADC_cutValue);
    h->Smooth(10, 2);

    qreal peakLoc = h->GetPeak().x();
    m_slope = peakE / peakLoc;
}

Histogram* Crystal::GetADCHist()
{
    return m_ADCHist;
}

void Crystal::SetADCHist(Histogram* aHist)
{
    m_ADCHist = aHist;
}

Histogram* Crystal::GetRecEHist()
{
    return m_recEHist;
}

void Crystal::SetRecEHist(Histogram* aHist)
{
    m_recEHist = aHist;
}

qreal Crystal::GetSlope() const
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
