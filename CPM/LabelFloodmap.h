#pragma once
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
	LabelFloodmap(QWidget* parent = nullptr);
	~LabelFloodmap();

	QPoint GetPos() const;

signals:
	void mouseLeftClicked();
	void mouseRightClicked();

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;

private:
	QPoint m_pos;
};