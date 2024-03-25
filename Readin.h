#ifndef READIN_H
#define READIN_H

#include <QObject>
#include <QString>
#include <QVector>


class Readin: public QObject
{
    Q_OBJECT

public:
    Readin();
    Readin(QString fName);
    Readin(QString fName, int CMN, int BKN);
    ~Readin();

    QVector< QVector<quint16> > GetData();

    void SetCMNumber(int n);
    void SetBKNumber(int n);


public slots:
    void StartReadTxt(); // readin text file
    void StartReadBin(); // readin binary file


signals:
    void currentPos(int pos);
    void finished();


private:
    QString m_fileName;
    QVector< QVector<quint16> > m_data;

    int nCM;
    int nBK;
};

#endif // READIN_H
