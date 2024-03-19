#include "floodmapLabel.h"


floodmapLabel::floodmapLabel(QWidget *parent)
    : QLabel(parent)
{

}


floodmapLabel::~floodmapLabel()
{

}


QPoint floodmapLabel::GetPos()
{
    return m_pos;
}


void floodmapLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 左键点击事件
        m_pos = event->pos();
        emit mouseLeftClicked();
    }
    else if (event->button() == Qt::RightButton)
    {
        emit mouseRightClicked();
    }

    // 保留 QLabel 默认的处理
    QLabel::mousePressEvent(event);
}

