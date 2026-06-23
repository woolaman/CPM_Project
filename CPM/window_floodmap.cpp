#include "window_floodmap.h"
#include "ui_window_floodmap.h"
#include "Parameters.h"

#include <opencv2/imgproc.hpp>

#include <QPainter>

window_floodmap::window_floodmap(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::window_floodmap)
{
	ui->setupUi(this);
}

window_floodmap::~window_floodmap()
{
	delete ui;
}

void window_floodmap::ShowPeaks()
{
	cv::Mat colorMap = GetColorMap(m_I0);
	//QImage qtImage(colorMap.data, colorMap.cols, colorMap.rows,
	//               colorMap.step, QImage::Format_BGR888);
	QImage qtImage(colorMap.data, colorMap.cols, colorMap.rows,
		colorMap.step, QImage::Format_RGB888);
	qtImage = qtImage.rgbSwapped();

	QPixmap pixmap = QPixmap::fromImage(qtImage);

	QPainter painter(&pixmap);
	painter.setPen(QPen(Qt::green, 1));

	for (int i = 0; i < nCrystal; ++i)
	{
		QVector<QPoint> points;
		for (int j = 0; j < nCrystal; ++j)
		{
			int x = m_pt(j, i)[0];
			int y = m_pt(j, i)[1];

			int r = 4;
			painter.drawLine(x - r, y, x + r, y); // "——"
			//painter.drawLine(x, y-r, x, y+r); // "|"
			points.append(QPoint(x, y));
		}
		painter.drawPolyline(points.data(), points.size());
	}

	ui->label_floodmap->setPixmap(pixmap);
}

// 一些功能性函数
cv::Mat window_floodmap::GetColorMap(cv::Mat_<qreal> I0)
{
	float c = 2.0f;
	cv::Mat_<quint8> I1(I0.size(), 0);

	qreal minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(I0, &minVal, &maxVal, &minLoc, &maxLoc);

	for (int i = 0; i < I0.rows; ++i)
	{
		for (int j = 0; j < I0.cols; ++j)
		{
			int value = qRound(1.0 * I0(i, j) / maxVal * 255 * c);
			if (value > 200)
			{
				value = 200;
			}
			I1(i, j) = static_cast<quint8>(value);
		}
	}

	double clipLimit = 2.0;
	cv::Size tileGridSize = cv::Size(8, 8);

	cv::Mat I2;
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(clipLimit);
	clahe->setTilesGridSize(tileGridSize);
	clahe->apply(I1, I2);

	cv::Mat I3;
	cv::applyColorMap(I2, I3, cv::COLORMAP_HOT);

	return I3;
}

void window_floodmap::mousePressEvent(QMouseEvent *event)
{
	// 检查是否是鼠标左键
	if (event->button() == Qt::LeftButton)
	{
		// 获取鼠标点击的全局位置，并转换为窗口内的坐标
		QPoint pos = event->pos();
		qDebug() << "Mouse clicked at: " << pos;

		cv::Mat_<qreal> dis(nCrystal, nCrystal, 0.0);
		for (int i = 0; i < nCrystal; ++i)
		{
			for (int j = 0; j < nCrystal; ++j)
			{
				cv::Point2f p1(m_pt(i, j)[0], m_pt(i, j)[1]);
				cv::Point2f p2(pos.x(), pos.y());
				dis(i, j) = cv::norm(p1 - p2);
			}
		}

		cv::Point min_loc;
		cv::minMaxLoc(dis, NULL, NULL, &min_loc, NULL);

		m_pt(min_loc)[0] = pos.x();
		m_pt(min_loc)[1] = pos.y();

		ShowPeaks();
	}

	// 检查是否是鼠标右键
	if (event->button() == Qt::RightButton)
	{
		for (int iRow = 0; iRow < nCrystal; ++iRow)
		{
			for (int iCol = 0; iCol < nCrystal; ++iCol)
			{
				cv::Point2f x(m_pt(iRow, iCol)[0], m_pt(iRow, iCol)[1]);
				cv::Point2f pre_x(0, 0);
				while (cv::norm(pre_x - x) > 0.1)
				{
					pre_x = x;
					cv::Point2f u(0, 0);
					qreal d = 0;

					int r = 2;
					for (int n = std::max(static_cast<int>(x.y - r), 1);
						n <= std::min(static_cast<int>(x.y + r), 1023); ++n)
					{
						for (int m = std::max(static_cast<int>(x.x - r), 1);
							m <= std::min(static_cast<int>(x.x + r), 1023); ++m)
						{
							if (cv::norm(x - cv::Point2f(m, n)) <= r)
							{
								//qreal k = cv::norm(x-cv::Point2f(m, n))/pow((2*r+1), 2);
								//qreal g = (1/sqrt(2*M_PI))*exp(-0.5*k);
								qreal g = 1;
								u += m_I0(n, m) * g * cv::Point2f(m, n);
								d += m_I0(n, m)*g;
							}
						}
					}
					x = u / d;
				}
				m_pt(iRow, iCol)[0] = qRound(x.x);
				m_pt(iRow, iCol)[1] = qRound(x.y);
			}
		}
		ShowPeaks();
	}
}

void window_floodmap::SetFloodmap(cv::Mat_<qreal> I)
{
	cv::resize(I, m_I0, cv::Size(1024, 1024), 0, 0, cv::INTER_LINEAR);
}

void window_floodmap::SetPeakTable(cv::Mat_<cv::Vec2w> pt)
{
	m_pt = 2 * pt;
}

cv::Mat_<qreal> window_floodmap::GetFloodmap()
{
	cv::Mat_<qreal> I;
	cv::resize(m_I0, I, cv::Size(512, 512), 0, 0, cv::INTER_LINEAR);
	return I;
}

cv::Mat_<cv::Vec2w> window_floodmap::GetPeakTable()
{
	return m_pt / 2;
}

void window_floodmap::closeEvent(QCloseEvent *event)
{
	cv::Mat_<qreal> I;
	cv::resize(m_I0, I, cv::Size(512, 512), 0, 0, cv::INTER_LINEAR);

	cv::Mat_<cv::Vec2w> pt = m_pt / 2;

	emit closeClicked(I, pt);
	QWidget::closeEvent(event);
}