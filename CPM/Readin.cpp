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

void Readin::StartReadData()
{
	//StartReadTxt();
	StartReadSingleDPB();
	//StartReadWholeRing(); // PET 环 8 Byte 数据格式
}

void Readin::StartReadTxt()
{
	QVector<QVector<quint16>> data(3);

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

		if (3 == numbers.size())
		{
			data[0].append(numbers[0].toInt());
			data[1].append(numbers[1].toInt());
			data[2].append(numbers[2].toInt());
		}
		else
		{
			qDebug() << "numbers size is not 3, data file is broken!";
			continue;
		}

		int pos = qRound(100.0 * file.pos() / fileSize);
		if (pos > prePos)
		{
			emit currentPos(pos);
			prePos = pos;
		}
	}
	file.close();
}

void Readin::StartReadSingleDPB()
{
	QMutexLocker locker(&m_mutex);

	// data format:
	quint8 B0; // CrystalID[11:8] & X"2"
	quint8 B1; // T[31:24]
	quint8 B2; // T[23:16]
	quint8 B3; // T[15:8]
	quint8 B4; // T[7:0]
	quint8 B5; // CMID[7:3] & BKID[2:0]
	quint8 B6; // X[15:8]
	quint8 B7; // X[7:0]
	quint8 B8; // Y[15:8]
	quint8 B9; // Y[7:0]
	quint8 BA; // E[15:8]
	quint8 BB; // E[7:0]
	quint8 BC; // XPOS[11:4]
	quint8 BD; // XPOS[3:0] & YPOS[11:8]
	quint8 BE; // YPOS[7:0]
	quint8 BF; // CrystalID[7:0]

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

	int prePos = 0;

	// 文件很大，分成 n 次读取，每次读取 1 G 大小
	quint64 fileSize = file.size(); // 读取文件大小

	// 计算每次读取的大小
	quint64 sizePerRead = 1024 * 1024 * 1024; // 1 G
	size_t nRead = fileSize / sizePerRead + 1; // 读取次数

	// 如果文件小于1 G，一次性读完
	if (fileSize < sizePerRead)
	{
		sizePerRead = fileSize;
		nRead = 1;
	}

	// 循环读取文件
	for (size_t i = 0; i < nRead; ++i)
	{
		m_data.clear();
		m_data = QVector<QVector<QVector<Event>>>(nCM, QVector<QVector<Event>>(nBK));

		// 读取文件数据
		quint64 bytesToRead = qMin(fileSize - file.pos(), sizePerRead);
		QByteArray buffer = file.read(bytesToRead);
		if (buffer.isEmpty())
		{
			break; // 读取结束
		}

		// 将读取的数据转换为 QDataStream
		QDataStream inBuffer(buffer);
		while (!inBuffer.atEnd())  // 循环读取每个事例的数据
		{
			// 读取 16 个变量的数据
			inBuffer >> B0 >> B1 >> B2 >> B3 >> B4 >> B5 >> B6 >> B7 >>
				B8 >> B9 >> BA >> BB >> BC >> BD >> BE >> BF;

			// 处理读取的数据
			quint8 type = B0 & 0x0F; // 保留后4位
			if (type != 2)
			{
				qDebug() << "type is not 2. break \n";
				continue;
			}

			int CMID = (B5 >> 3);
			if (CMID<0 || CMID>nCM)
			{
				qDebug() << "CMID is out of range! \n";
				continue;
			}

			int BKID = (B5 & 0x07);
			if (BKID<0 || BKID>nBK)
			{
				qDebug() << "BKID is out of range! \n";
				continue;
			}

			Event aEvt{};
			aEvt.e = (static_cast<quint16>(BA) << 8) + BB;
			aEvt.x = (static_cast<quint16>(BC) << 4) + (BD >> 4);
			aEvt.y = (static_cast<quint16>(BD & 0x0F) << 8) + BE;
			m_data[CMID][BKID].append(aEvt);

			// 计算当前读取的进度
			quint64 filePos = file.pos() - bytesToRead + inBuffer.device()->pos();
			int pos = qRound(100.0 * filePos / fileSize);
			if (pos > prePos)
			{
				emit currentPos(pos);
				prePos = pos;
			}
		}

		// 读取完成，保存数据到文件
		for (size_t iCM = 0; iCM < nCM; iCM++)
		{
			for (size_t iBK = 0; iBK < nBK; iBK++)
			{
				QString str_CMID = QString::number(iCM).rightJustified(2, '0');
				QString str_BKID = QString::number(iBK).rightJustified(2, '0');
				QString fName = currentPath + "Data/s_" + str_CMID + "_" + str_BKID;
				QFile ofile(fName);

				if (ofile.open(QIODevice::WriteOnly | QIODevice::Append))
				{
					QDataStream out(&ofile);
					QVector<Event> v = m_data[iCM][iBK];
					for (const auto& aEvt : v)
					{
						quint16 x = aEvt.x;
						quint16 y = aEvt.y;
						quint16 e = aEvt.e;

						if (0 < x && x < nPixel && 0 < y && y < nPixel && 0 < e && e < 65535)
						{
							out << x << y << e;
						}
					}
					ofile.close();
				}
				else
				{
					qDebug() << "Fail to create readout file.";
					return;
				}
			}
		}
	}

	// 关闭文件
	file.close();
	emit ShowMessage(QString::fromLocal8Bit("读入数据"), QString::fromLocal8Bit("读入数据完成。"));
}

void Readin::StartReadWholeRing()
{
	QMutexLocker locker(&m_mutex);

	// data format:
	quint8 B0;  // TYPE: [3:0]=3, [7:4]保留
	quint8 B1;  // [7:3]=CMID, [2:0]=BKID
	quint8 B2;  // Xpos[8:1]
	quint8 B3;  // Ypos[8:1]
	quint8 B4;  // Xpos[0] Ypos[0], [5:0]保留
	quint8 B5;  // 保留
	quint8 B6;  // Energy[15:8]
	quint8 B7;  // Energy[7:0]						

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

	int prePos = 0;

	// 文件很大，分成 n 次读取，每次读取 1 G 大小
	quint64 fileSize = file.size(); // 读取文件大小

	// 计算每次读取的大小
	quint64 sizePerRead = 1024 * 1024 * 1024; // 1 G
	size_t nRead = fileSize / sizePerRead + 1; // 读取次数

	// 如果文件小于1 G，一次性读完
	if (fileSize < sizePerRead)
	{
		sizePerRead = fileSize;
		nRead = 1;
	}

	// 循环读取文件
	for (size_t i = 0; i < nRead; ++i)
	{
		m_data.clear();
		m_data = QVector<QVector<QVector<Event>>>(nCM, QVector<QVector<Event>>(nBK));

		// 读取文件数据
		quint64 bytesToRead = qMin(fileSize - file.pos(), sizePerRead);
		QByteArray buffer = file.read(bytesToRead);
		if (buffer.isEmpty())
		{
			break; // 读取结束
		}

		// 将读取的数据转换为 QDataStream
		QDataStream inBuffer(buffer);
		while (!inBuffer.atEnd())  // 循环读取每个事例的数据
		{
			// 读取 8 个变量的数据
			inBuffer >> B0 >> B1 >> B2 >> B3 >> B4 >> B5 >> B6 >> B7;

			// 处理读取的数据
			quint8 type = B0 & 0x0F; // 保留后4位
			if (type != 3)
			{
				qDebug() << "type is not 3. break \n";
				continue;
			}

			int CMID = (B1 >> 3); // 提取高 5 位
			if (CMID < 0 || CMID >= nCM)
			{
				qDebug() << "CMID is out of range! \n";
				continue;
			}

			int BKID = (B1 & 0x07); //保留后 3 位
			if (BKID < 0 || BKID >= nBK)
			{
				qDebug() << "BKID is out of range! \n";
				continue;
			}

			Event aEvt{};
			aEvt.x = (static_cast<quint16>(B2) << 1) + (B4 >> 7);
			aEvt.y = (static_cast<quint16>(B3) << 1) + ((B4 >> 6) & 0x01);
			aEvt.e = (static_cast<quint16>(B6) << 8) + B7;
			m_data[CMID][BKID].append(aEvt);

			// 计算当前读取的进度
			quint64 filePos = file.pos() - bytesToRead + inBuffer.device()->pos();
			int pos = qRound(100.0 * filePos / fileSize);
			if (pos > prePos)
			{
				emit currentPos(pos);
				prePos = pos;
			}
		}

		// 读取完成，保存数据到文件
		for (size_t iCM = 0; iCM < nCM; iCM++)
		{
			for (size_t iBK = 0; iBK < nBK; iBK++)
			{
				QString str_CMID = QString::number(iCM).rightJustified(2, '0');
				QString str_BKID = QString::number(iBK).rightJustified(2, '0');
				QString fName = currentPath + "Data/s_" + str_CMID + "_" + str_BKID;
				QFile ofile(fName);

				if (ofile.open(QIODevice::WriteOnly | QIODevice::Append))
				{
					QDataStream out(&ofile);
					QVector<Event> v = m_data[iCM][iBK];
					for (const auto& aEvt : v)
					{
						quint16 x = aEvt.x;
						quint16 y = aEvt.y;
						quint16 e = aEvt.e;

						if (0 < x && x < nPixel && 0 < y && y < nPixel && 0 < e && e < 65535)
						{
							out << x << y << e;
						}
					}
					ofile.close();
				}
				else
				{
					qDebug() << "Fail to create readout file.";
					return;
				}
			}
		}
	}

	// 关闭文件
	file.close();
	emit ShowMessage(QString::fromLocal8Bit("读入数据"), QString::fromLocal8Bit("读入数据完成。"));
}

const QVector< QVector< QVector<Event> > >& Readin::GetData() const
{
	return m_data;
}

void Readin::Clear()
{
	m_data.clear();
}