#include "Block.h"


Block::Block()
{

}


Block::~Block()
{

}


void Block::Append(Crystal aCrystal)
{
    m_crystals.append(aCrystal);
}



Histogram Block::GetADCHist()
{

    // magic here


    return m_ADCHist;

}



Histogram Block::GetEHist()
{
    // magic here

    return m_eHist;
}









