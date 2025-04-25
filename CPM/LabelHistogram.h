#ifndef EHISTLABEL_H
#define EHISTLABEL_H

#include <QLabel>
#include <QObject>
#include <QPoint>
#include <QMouseEvent>


class LabelHistogram : public QLabel
{
    Q_OBJECT

public:
    LabelHistogram(QWidget *parent = nullptr);
    ~LabelHistogram();

    QPoint GetLeftPos();
    QPoint GetRightPos();


signals:
    void mouseLeftClicked();
    void mouseRightClicked();


protected:
    void mousePressEvent(QMouseEvent *event) override;


private:
    QPoint m_leftPos;
    QPoint m_rightPos;
};

#endif // EHISTLABEL_H
