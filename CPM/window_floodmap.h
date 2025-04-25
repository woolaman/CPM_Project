#ifndef WINDOW_FLOODMAP_H
#define WINDOW_FLOODMAP_H

#include <QWidget>
#include <QMouseEvent>
#include <QDebug>

#include "opencv2/core/mat.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace Ui
{
class window_floodmap;
}

class window_floodmap : public QWidget
{
    Q_OBJECT

public:
    explicit window_floodmap(QWidget *parent = nullptr);
    ~window_floodmap();

	cv::Mat GetColorMap(cv::Mat_<qreal> I);
	void ShowPeaks();

	void SetFloodmap(cv::Mat_<qreal> I);
	void SetPeakTable(cv::Mat_<cv::Vec2w> pt);

	cv::Mat_<qreal> GetFloodmap();
	cv::Mat_<cv::Vec2w> GetPeakTable();

signals:
	void closeClicked(cv::Mat_<qreal> I, cv::Mat_<cv::Vec2w> pt);

private slots:


protected:
    void closeEvent(QCloseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::window_floodmap *ui;

	cv::Mat_<qreal> m_I0;
	cv::Mat_<cv::Vec2w> m_pt;
};

#endif // WINDOW_FLOODMAP_H