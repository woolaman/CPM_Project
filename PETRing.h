#ifndef PETRING_H
#define PETRING_H

#include <QVector>

#include "Module.h"

class PETRing
{
public:
    PETRing();
	~PETRing();
	
	
private:
    QVector<Module*> m_modules;

};

#endif // PETRING_H
