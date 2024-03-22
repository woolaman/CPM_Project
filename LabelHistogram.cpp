#include "LabelHistogram.h"


LabelHistogram::LabelHistogram(QWidget *parent)
    : QLabel(parent)
{
    m_leftPos = QPoint(0, 0);
    m_rightPos = QPoint(0, 0);
}

LabelHistogram::~LabelHistogram()
{

}


QPoint LabelHistogram::GetLeftPos()
{
    return m_leftPos;
}


QPoint LabelHistogram::GetRightPos()
{
    return m_rightPos;
}


void LabelHistogram::mousePressEvent(QMouseEvent *event)
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

