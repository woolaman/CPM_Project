#include "Module.h"

#include "Parameters.h"

Module::Module()
{
    for (int i = 0; i < nBK; ++i)
    {
        Block* aBK = new Block(i);
        m_BKs.append(aBK);
    }
}


Module::Module(int id)
{
    m_ID = id;
    for (int i = 0; i < nBK; ++i)
    {
        Block* aBK = new Block(i);
        m_BKs.append(aBK);
    }
}






