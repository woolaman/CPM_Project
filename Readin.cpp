#include "Readin.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>


Readin::Readin() {}


Readin::Readin(QString fName)
{
    m_fileName = fName;
}


Readin::Readin(QString fName, int CMN, int BKN)
{
    m_fileName = fName;
    nCM = CMN;
    nBK = BKN;
}


Readin::~Readin() {}


void Readin::StartReadTxt()
{
    m_data = QVector< QVector<quint16> >(3);

    if (!QFile::exists(m_fileName)) // 检查文件是否存在
    {
        qDebug() << "file name is " << m_fileName;
        qDebug() << "Fail to open file, please check file.";
        return;
    }

    QFile file(m_fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Fail to open file, please check file.";
        return;
    }

    quint64 fileSize = file.size(); // 获取文件大小，用于计算进度

    QTextStream in(&file);
    int prePos = 0;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList numbers = line.split(" ", Qt::SkipEmptyParts);

        if(3==numbers.size())
        {
            m_data[0].append(numbers[0].toInt());
            m_data[1].append(numbers[1].toInt());
            m_data[2].append(numbers[2].toInt());
        }
        else
        {
            qDebug() << "numbers size is not 3, data file is broken!";
            continue;
        }

        int pos = qRound(100.0*file.pos()/fileSize);
        if(pos>prePos)
        {
            emit currentPos(pos);
            prePos = pos;
        }
    }
    file.close();

    emit finished();
}


void Readin::StartReadBin()
{
    m_data = QVector< QVector<quint16> >(nCM*nBK);

    // 检查文件是否存在
    if (!QFile::exists(m_fileName))
    {
        qDebug() << "Fail to open file, please check file.";
        return;
    }

    quint8 type;     // type:[3:0], [7:4]保留
    quint8 t_high;   // t[15:8]
    quint8 t_low;    // t[7:0]
    quint8 sectorID; // [7:4]CMID, [3:1]BKID
    quint8 xPos;     // x[8:1]
    quint8 yPos;     // y[8:1]
    quint8 e_high;   // [7]为x[0]; [6]为y[0], [3:0]为E[11:8]
    quint8 e_low;    // E[7:0]

    QFile file(m_fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error opening file!";
        return;
    }

    quint64 fileSize = file.size(); // 获取文件大小，用于计算进度

    QDataStream in(&file);
    while (!in.atEnd())  // 循环读取每个事例的数据
    {
        // 读取8个变量的数据
        in >> type >> t_high >> t_low >> sectorID >>
            xPos >> yPos >> e_high >> e_low;

        // 处理读取的数据
        if(type!=3)
        {
            qDebug() << "type is not 3. break \n";
            continue;
        }

        int CMID = (sectorID >> 4);
        if(CMID<0 || CMID>nCM)
        {
            qDebug() << "CMID is out of range! \n";
            continue;
        }

        int BKID = ((sectorID >> 1) & 0x07);
        if(BKID<0 || BKID>nBK)
        {
            qDebug() << "BKID is out of range! \n";
            continue;
        }

        int iLine = CMID*nBK + BKID;
        if(iLine<0 || iLine>nCM*nBK)
        {
            qDebug() << "total BKID is out of range! \n";
            continue;
        }

        quint16 x = xPos*2 + (e_high>>7);
        quint16 y = yPos*2 + ( (e_high>>6) & 0x01);
        quint16 e = (e_high&0x0F)*std::pow(2,8) + e_low;

        m_data[iLine].append(x);
        m_data[iLine].append(y);
        m_data[iLine].append(e);

        double p1 = 100.0*file.pos()/fileSize;
        int p2 = qRound(p1);
        if(qFabs(p1-p2)<1.e-5)
        {
            emit currentPos(p2);
        }
    }
    file.close();

    emit finished();
}


QVector< QVector<quint16> > Readin::GetData()
{
    return m_data;
}


void Readin::SetCMNumber(int n)
{
    nCM = n;
}


void Readin::SetBKNumber(int n)
{
    nBK = n;
}

