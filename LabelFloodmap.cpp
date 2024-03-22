#include "LabelFloodmap.h"


LabelFloodmap::LabelFloodmap(QWidget *parent)
    : QLabel(parent)
{

}


LabelFloodmap::~LabelFloodmap()
{

}


QPoint LabelFloodmap::GetPos()
{
    return m_pos;
}


void LabelFloodmap::mousePressEvent(QMouseEvent *event)
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

