#include "Readin.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDataStream>
#include <QString>

#include "Parameters.h"

Readin::Readin() {}

Readin::Readin(QString fName)
{
    m_fileName = fName;
}

Readin::~Readin() {}

void Readin::StartReadTxt()
{
    /**************************************************
    m_data = QVector< QVector<quint16> >(3);

    if (!QFile::exists(m_fileName)) // 检查文件是否存在
    {
        qDebug() << "File doesnot exists, please check file: " << m_fileName;
        return;
    }

    QFile file(m_fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Fail to open file, please check file: " << m_fileName;
        return;
    }

    quint64 fileSize = file.size(); // 获取文件大小，用于计算进度

    QTextStream in(&file);
    int prePos = 0;
    while (!in.atEnd())
    {
        QString line = in.readLine();
		QStringList numbers = line.split(" ", QString::SkipEmptyParts);

        if(3==numbers.size())
        {
            m_data[0].append(numbers[0].toInt() - bias); // 注意！
            m_data[1].append(numbers[1].toInt() - bias); // 注意！
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
    ***************************************************/
}

void Readin::StartReadBin()
{
    //StartReadBin1();
    //StartReadBin2();
	StartReadBin3();
}

void Readin::StartReadBin1()
{
    /*****************************************************
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
    int prePos = 0;
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

        int pos = qRound(100.0*file.pos()/fileSize);
        if(pos>prePos)
        {
            emit currentPos(pos);
            prePos = pos;
        }
    }
    file.close();

    emit finished();
	*********************************************/
}

void Readin::StartReadBin2()
{
	m_data = QVector<QVector<QVector<Event>>>(nCM, QVector<QVector<Event>>(nBK));

	// data format:
	quint8 B15; // byte 15: CrystalID[11:8] & X"2"
	quint8 B14; // byte 14: T[31:24]
	quint8 B13; // byte 13: T[23:16]
	quint8 B12; // byte 12: T[15:8]
	quint8 B11; // byte 11: T[7:0]
	//quint8 B10; // byte 10: CMID[7:4] & BKID[3:0]
	quint8 B10; // byte 10: CMID[7:3] & BKID[2:0]
	quint8 B9;  // byte 9:  X[15:8]
	quint8 B8;  // byte 8:  X[7:0]
	quint8 B7;  // byte 7:  Y[15:8]
	quint8 B6;  // byte 6:  Y[7:0]
	quint8 B5;  // byte 5:  E[15:8]
	quint8 B4;  // byte 4:  E[7:0]
	quint8 B3;  // byte 3:  XPOS[11:4]
	quint8 B2;  // byte 2:  XPOS[3:0] & YPOS[11:8]
	quint8 B1;  // byte 1:  YPOS[7:0]
	quint8 B0;  // byte 0:  CrystalID[7:0]						

	// 检查文件是否存在
	if (!QFile::exists(m_fileName))
	{
		qDebug() << "Fail to open file, please check file.";
		return;
	}

	QFile file(m_fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Error opening file!";
		return;
	}

	quint64 fileSize = file.size(); // 获取文件大小，用于计算进度

	QDataStream in(&file);
	int prePos = 0;
	while (!in.atEnd())  // 循环读取每个事例的数据
	{
		// 读取 16 个变量的数据
		in >> B15 >> B14 >> B13 >> B12 >> B11 >> B10 >> B9 >> B8 >>
			B7 >> B6 >> B5 >> B4 >> B3 >> B2 >> B1 >> B0;

		// 处理读取的数据
		quint8 type = B15 & 0x0F; // 保留后4位
		if (type != 2)
		{
			qDebug() << "type is not 2. break \n";
			continue;
		}

		//int CMID = (B10 >> 4);
		int CMID = (B10 >> 3);
		if (CMID<0 || CMID>nCM)
		{
			qDebug() << "CMID is out of range! \n";
			continue;
		}

		//int BKID = (B10 & 0x0F);
		int BKID = (B10 & 0x07);
		if (BKID<0 || BKID>nBK)
		{
			qDebug() << "BKID is out of range! \n";
			continue;
		}

		Event aEvt;
		aEvt.x = (static_cast<quint16>(B3) << 4) + (B2 >> 4);
		aEvt.y = (static_cast<quint16>(B2 & 0x0F) << 8) + B1;
		aEvt.e = (static_cast<quint16>(B5) << 8) + B4;

		m_data[CMID][BKID].append(aEvt);

		int pos = qRound(100.0*file.pos() / fileSize);
		if (pos > prePos)
		{
			emit currentPos(pos);
			prePos = pos;
		}
	}
	file.close();

	emit finished();
}

void Readin::StartReadBin3()
{
	m_data = QVector<QVector<QVector<Event>>>(nCM, QVector<QVector<Event>>(nBK));

	// data format:
	quint8 B7;  // TYPE: [3:0]=3, [7:4]保留
	quint8 B6;  // SectorID: [7:3]=CMID, [2:0]=BKID
	quint8 B5;  // Xpos[8:1]
	quint8 B4;  // Ypos[8:1]
	quint8 B3;  // Xpos[0] Ypos[0], [5:0]保留
	quint8 B2;  // 保留
	quint8 B1;  // Energy[15:8]
	quint8 B0;  // Energy[7:0]						

	// 检查文件是否存在
	if (!QFile::exists(m_fileName))
	{
		qDebug() << "Fail to open file, please check file.";
		return;
	}

	QFile file(m_fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Error opening file!";
		return;
	}

	quint64 fileSize = file.size(); // 获取文件大小，用于计算进度

	QDataStream in(&file);
	int prePos = 0;

	while (!in.atEnd())  // 循环读取每个事例的数据
	{
		// 读取 16 个变量的数据
		in >> B7 >> B6 >> B5 >> B4 >> B3 >> B2 >> B1 >> B0;

		// 处理读取的数据
		quint8 type = B7 & 0x0F; // 保留后4位
		if (type != 3)
		{
			qDebug() << "type is not 3. break \n";
			continue;
		}

		int CMID = (B6 >> 3);
		if (CMID<0 || CMID>nCM)
		{
			qDebug() << "CMID is out of range! \n";
			continue;
		}

		int BKID = (B6 & 0x07); //保留后 3 位
		if (BKID<0 || BKID>nBK)
		{
			qDebug() << "BKID is out of range! \n";
			continue;
		}

		Event aEvt;
		aEvt.x = (static_cast<quint16>(B5) << 1) + (B3 >> 7);
		aEvt.y = (static_cast<quint16>(B4) << 1) + ((B3 >> 6) & 0x01);
		aEvt.e = (static_cast<quint16>(B1) << 8) + B0;

		m_data[CMID][BKID].append(aEvt);

		int pos = qRound(100.0*file.pos() / fileSize);
		if (pos > prePos)
		{
			emit currentPos(pos);
			prePos = pos;
		}
	}
	file.close();

	emit finished();
}

const QVector< QVector< QVector<Event> > >& Readin::GetData() const
{
    return m_data;
}

void Readin::Clear()
{
    m_data.clear();
}