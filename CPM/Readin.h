#ifndef READIN_H
#define READIN_H

#include <QObject>
#include <QString>
#include <QVector>

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
    void StartReadTxt(); // readin text file
    void StartReadBin(); // readin binary file
    void StartReadBin1(); // 8 bytes per data package
    void StartReadBin2(); // 16 bytes per data package
	void StartReadBin3(); // Readin whole PET ring data

signals:
    void currentPos(int pos);
    void finished();

private:
    QString m_fileName;
    QVector< QVector< QVector<Event> > > m_data;
};

#endif // READIN_H