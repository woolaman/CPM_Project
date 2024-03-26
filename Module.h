#ifndef MODULE_H
#define MODULE_H

#include "Block.h"

class Module
{
public:
    Module();
    Module(int id);
    ~Module();


private:
    int m_ID;
    QVector<Block*> m_BKs;

};

#endif // MODULE_H
