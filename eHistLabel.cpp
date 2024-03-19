#include "eHistLabel.h"


eHistLabel::eHistLabel(QWidget *parent)
    : QLabel(parent)
{
    m_leftPos = QPoint(0, 0);
    m_rightPos = QPoint(0, 0);
}

eHistLabel::~eHistLabel()
{

}


QPoint eHistLabel::GetLeftPos()
{
    return m_leftPos;
}


QPoint eHistLabel::GetRightPos()
{
    return m_rightPos;
}


void eHistLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_leftPos = event->pos();
        emit mouseLeftClicked();
    }
    else if (event->button() == Qt::RightButton)
    {
        m_rightPos = event->pos();
        emit mouseRightClicked();
    }

    // 保留 QLabel 默认的处理
    QLabel::mousePressEvent(event);
}

