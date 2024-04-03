#include "LabelFloodmap.h"

#include <QCursor>


LabelFloodmap::LabelFloodmap(QWidget *parent)
    : QLabel(parent)
{
    m_pos = QPoint(0, 0);
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


void LabelFloodmap::mouseMoveEvent(QMouseEvent *event)
{
    QPoint localPos = event->pos();
    QPoint cursorPos = QCursor::pos();
    QToolTip::showText(cursorPos + QPoint(10, 10),
                       QString("(%1, %2)").arg(localPos.x()).arg(localPos.y()),
                       this);
    QLabel::mouseMoveEvent(event);
}

