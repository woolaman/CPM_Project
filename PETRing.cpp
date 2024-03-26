#include "PetRing.h"

#include "Parameters.h"


PETRing::PETRing()
{
    for (int i = 0; i < nCM; ++i)
    {
        Module* aCM = new Module(i);
        m_modules.append(aCM);
    }
}


PETRing::~PETRing()
{

}














