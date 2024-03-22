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


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    cv::Mat GetColorMap(cv::Mat_<float> I);
    void ShowImage(cv::Mat_<float> I);
    void ShowPeaks(cv::Mat_<float> I, cv::Mat_<cv::Vec2w> pt);

    void ShowEHist(int peakLoc=-1);

    float GetER(QVector<int>& y);

    QVector<int> FindPeaks(QVector<float> v, int nPeaks);

    void LogOut(QString str);

    template<typename T>
    QVector<float> Smooth(const QVector<T>& data, int window=10);

    void ReadinParFile(QString fName="cpm_config.par");


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

    Readin* dataObject;

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
    QVector<double> m_slopes;
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

    QMap<QString, QString> pars;
    int nCM;
    int nBK;
    int nPixel;
    int nCrystal;
    int num1; // nCrystal = num1 * num2
    int num2;
    int crystalNum;

    float factor; // 调整点阵大小
    int bias;  // 调整点阵位置

    // choose central area to analysis
    int xmin; //int xmin = 0;
    int xmax; //int xmax = 512;
    int ymin; //int ymin = 0;
    int ymax; //int ymax = 512;

    QString segmentMethod;

    float EW_width;

    int minADC;
    int maxADC; // ADC channel value
    int ADC_nBins;
    int ADC_binWidth;
    int ADC_cutValue; // 将小于该值的bin置为0，防止寻峰错误

    int minRecE;
    int maxRecE; // keV
    int recE_nBins;
    int recE_binWidth;
    int recE_cutValue; // 将小于该值的bin置为0，防止寻峰错误
};

#endif // MAINWINDOW_H
