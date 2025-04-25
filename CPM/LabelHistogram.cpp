/**
 * 用于显示能谱的label，继承自QLabel，重新实现了鼠标点击操作。
 * 左键和左键点击后返回鼠标点击位置。
*/


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

    QLabel::mousePressEvent(event);
}

