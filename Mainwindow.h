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
    void LogOut(QString str);


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


private:
    Ui::MainWindow* ui;

    //oooooooOOOOOOOOOoooooooooooo // 能谱相关
    QLineSeries* eHistLine;
    QLineSeries* EWLeftLine;
    QLineSeries* EWRightLine;
    QLineSeries* peakLine;
    QValueAxis*  axisX;
    QValueAxis*  axisY;
    QChart*      chart;
    QChartView*  chartView;

    qreal m_peakValue;
    //oooooooOOOOOOOOOoooooooooooo

    // 0-floodmap, 1-calPeaks, 2-calOnePeak
    quint8 imgFlag = 0;

    QFile* logFile;
    QTextStream* logOutStream;

    Readin* dataObject;
    Block* m_BK;
};

#endif // MAINWINDOW_H
