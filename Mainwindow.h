#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineSeries>
#include <QChart>
#include <QChartView>
#include <QValueAxis>
#include <QVector>

#include "opencv2/core/mat.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "Readin.h"
#include "Block.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    cv::Mat GetColorMap(cv::Mat_<qreal> I);
    void ShowImage(cv::Mat_<qreal> I);
    void ShowPeaks(cv::Mat_<qreal> I, cv::Mat_<cv::Vec2w> pt);
    void ShowADCHist(int peakLoc=-1);
    void ShowRecEHist();


private slots:
    void on_pushButton_readinData_clicked();
    void on_pushButton_setEW_clicked();
    void on_pushButton_segment_clicked();
    void on_pushButton_genPositionLUT_clicked();
    void on_pushButton_genEnergyLUT_clicked();
    void on_pushButton_calPeaks_clicked();
    void on_pushButton_calOnePeak_clicked();
    void on_pushButton_writePeaks_clicked();

    void on_label_eHist_mouseLeftClicked();
    void on_label_eHist_mouseRightClicked();

    void on_label_floodmap_mouseLeftClicked();
    void on_label_floodmap_mouseRightClicked();

    void on_pushButton_calSegFOM_clicked();

    void on_pushButton_calEnergyResolution_clicked();
    void on_pushButton_calUniformity_clicked();

    void on_lineEdit_minEW_editingFinished();
    void on_lineEdit_maxEW_editingFinished();

    void UpdateProgressBar(int pos);
    void ReStoreData();

    void on_pushButton_crystalES_clicked();


private:
    Ui::MainWindow* ui;

    Readin* dataObject;
    Block* m_BK;

    // energy histogram
    //oooooooOOOOOOOOOoooooooooooo
    QtCharts::QLineSeries* eHistLine;
    QtCharts::QLineSeries* EWLeftLine;
    QtCharts::QLineSeries* EWRightLine;
    QtCharts::QLineSeries* peakLine;
    QtCharts::QValueAxis*  axisX;
    QtCharts::QValueAxis*  axisY;
    QtCharts::QChart*      chart;
    QtCharts::QChartView*  chartView;

    qreal m_peakValue = 0;
    //oooooooOOOOOOOOOoooooooooooo

    // 0-floodmap, 1-calPeaks, 2-calOnePeak,
    // 3-check single crystal energy spectrum
    quint8 imgFlag = 0;
};

#endif // MAINWINDOW_H
