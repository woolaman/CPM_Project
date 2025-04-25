#ifndef LABELFLOODMAP_H
#define LABELFLOODMAP_H

#include <QApplication>
#include <QLabel>
#include <QObject>
#include <QPoint>
#include <QMouseEvent>
#include <QToolTip>


class LabelFloodmap : public QLabel
{
    Q_OBJECT


public:
    LabelFloodmap(QWidget *parent = nullptr);
    ~LabelFloodmap();

    QPoint GetPos();


signals:
    void mouseLeftClicked();
    void mouseRightClicked();


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;


private:
    QPoint m_pos;
};

#endif // LABELFLOODMAP_H
