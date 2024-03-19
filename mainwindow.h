#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineSeries>
#include <QChart>
#include <QChartView>
#include <QValueAxis>
#include <QVector>

//#include "opencv2/opencv.hpp"
#include "opencv2/core/mat.hpp"
//#include "opencv2/highgui/highgui.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    const int nCM = 1;
    const int nBK = 1;
    const int nPixel = 512;
    const int nCrystal = 14; // 26
    const int num1 = 7; // nCrystal = num1 * num2
    const int num2 = 2;
    const int crystalNum = nCrystal*nCrystal;

    const float factor = 1.06f; // 调整点阵大小
    const int bias = 58;  // 调整点阵位置

    const int xmin = 126;  // choose central area to analysis
    const int xmax = 383;
    const int ymin = 132;
    const int ymax = 383;
    //const int xmin = 0;  // choose central area to analysis
    //const int xmax = 512;
    //const int ymin = 0;
    //const int ymax = 512;

    const float EW_width = 0.25;

    const int minADC = 0;
    const int maxADC = 60000; // ADC channel value
    const int ADC_nBins = 600;
    const int ADC_binWidth = (maxADC-minADC)/ADC_nBins;
    const int ADC_cutValue = 15000;
    //将小于该值的bin置为0，防止寻峰错误

    const int minRecE = 0;
    const int maxRecE = 1600; // keV
    const int recE_nBins = 400;
    const int recE_binWidth = (maxRecE-minRecE)/recE_nBins;
    const int recE_cutValue = 200;
    //将小于该值的bin置为0，防止寻峰错误

    cv::Mat GetColorMap(cv::Mat_<float> I);
    void ShowImage(cv::Mat_<float> I);
    void ShowPeaks(cv::Mat_<float> I, cv::Mat_<cv::Vec2w> pt);

    void ShowEHist(int peakLoc=-1);

    float GetER(QVector<int>& y);

    QVector<int> FindPeaks(QVector<float> v, int nPeaks);

    void LogOut(QString str);

    template<typename T>
    QVector<float> Smooth(const QVector<T>& data, int window=10);


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


private:
    Ui::MainWindow* ui;

    //oooooooOOOOOOOOOoooooooooooo // 原始数据
    QVector<quint16> m_xList;
    QVector<quint16> m_yList;
    QVector<quint16> m_eList;
    //oooooooOOOOOOOOOoooooooooooo

    //oooooooOOOOOOOOOoooooooooooo // 能谱相关
    QLineSeries* eHistLine;
    QLineSeries* EWLeftLine;
    QLineSeries* EWRightLine;
    QLineSeries* peakLine;
    QValueAxis*  axisX;
    QValueAxis*  axisY;
    QChart*      chart;
    QChartView*  chartView;

    float m_peakValue;
    QVector< QVector<quint16> > m_eHists;
    QVector<quint16> m_peaks;
    //oooooooOOOOOOOOOoooooooooooo

    //oooooooOOOOOOOOOoooooooooooo // 分割相关
    cv::Mat_<float> I0; // nPixel*nPixel
    cv::Mat_<cv::Vec2w> pt; // nCrystal*nCrystal
    //oooooooOOOOOOOOOoooooooooooo

    // 0-floodmap, 1-calPeaks, 2-calOnePeak
    quint8 imgFlag = 0;

    QFile* logFile;
    QTextStream* logOutStream;

    QString currentPath;
    QString fName_LUT_P = "LUT_Position.bin";
    QString fName_LUT_E = "LUT_Energy.bin";
    QString fName_LUT_U = "LUT_Uniformity.bin";

    QVector<float> ADCs;
    QVector<float> recEs;
};

#endif // MAINWINDOW_H
