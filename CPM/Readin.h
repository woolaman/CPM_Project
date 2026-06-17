#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QMutex>
#include <QMutexLocker>

struct Event
{
    quint16 x;
    quint16 y;
    quint16 e;
};

class Readin: public QObject
{
    Q_OBJECT

public:
    Readin();
    Readin(QString fName);
    ~Readin();

    const QVector< QVector< QVector<Event> > >& GetData() const;
    void Clear();

public slots:
    void StartReadData(); // readin binary file
    void StartReadTxt(); // readin text file 
    void StartReadSingleDPB(); // 16 bytes per data package
	void StartReadWholeRing(); // Readin whole PET ring data

signals:
    void currentPos(int pos);
    void ShowMessage(QString title, QString content);

private:
    QString m_fileName;
    QVector< QVector< QVector<Event> > > m_data;
    mutable QMutex m_mutex;
};