#ifndef FLOODMAPLABEL_H
#define FLOODMAPLABEL_H

#include <QApplication>
#include <QLabel>
#include <QObject>
#include <QPoint>
#include <QMouseEvent>


class floodmapLabel : public QLabel
{
    Q_OBJECT

public:
    floodmapLabel(QWidget *parent = nullptr);
    ~floodmapLabel();

    QPoint GetPos();

signals:
    void mouseLeftClicked();
    void mouseRightClicked();


protected:
    void mousePressEvent(QMouseEvent *event) override;


private:
    QPoint m_pos;
};

#endif // FLOODMAPLABEL_H
