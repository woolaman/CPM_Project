#ifndef BLOCK_H
#define BLOCK_H


#include "Crystal.h"


class Block
{
public:
    Block();
    ~Block();

    int GetID();
    void SetID(int ID);


    Histogram GetADCHist();
    Histogram GetEHist();


    void Append(Crystal aCrystal);



private:

    int m_BKID;

    QVector<Crystal> m_crystals;

    Histogram m_ADCHist;
    Histogram m_eHist;

};

#endif // BLOCK_H
