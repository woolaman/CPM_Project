#include "Mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QPixmap>
#include <QPainter>
#include <QDateTime>
#include <QDir>
#include <QMessageBox>
#include <QThread>

#include <algorithm>
#include <math.h>
#include <numeric>
#include <iostream>

#include <opencv2/imgproc.hpp>

#include "Readin.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // 创建或打开日志文件（若不存在则会自动创建）
    logFile = new QFile("log.txt");
    if (logFile->open(QIODevice::WriteOnly))
    {
        logOutStream = new QTextStream(logFile);
    }
    else
    {
        qDebug() << "无法打开日志文件";
    }

    LogOut("程序初始化开始...");

    ui->setupUi(this);

    QDir dir(QCoreApplication::applicationDirPath()); // 获取当前程序目录
    currentPath = dir.absolutePath() + "/";

    //判断当前目录中是否存在文件夹"Data/"，如果不存在则创建一个
    QString directoryName = "Data";

    // 拼接目标文件夹的路径
    QString targetDirectoryPath = dir.absoluteFilePath(directoryName);

    // 检查文件夹是否存在，如果不存在则创建文件夹
    if (!dir.exists(directoryName))
    {
        if (dir.mkdir(directoryName))
        {
            std::cout << "Data directory created successfully: ";
            std::cout << targetDirectoryPath.toStdString() << std::endl;
        }
        else
        {
            std::cout << "Failed to create data directory: ";
            std::cout << targetDirectoryPath.toStdString() << std::endl;
        }
    }

    ReadinParFile();
    nCM = pars["nCM"].toInt();
    nBK = pars["nBK"].toInt();
    nPixel = pars["nPixel"].toInt();
    nCrystal = pars["nCrystal"].toInt();
    num1 = pars["num1"].toInt();
    num2 = pars["num2"].toInt();

    factor = pars["enlarge"].toFloat(); // 调整点阵大小
    bias = pars["bias"].toInt();  // 调整点阵位置

    // choose central area to analysis
    xmin = pars["xmin"].toInt(); //int xmin = 0;
    xmax = pars["xmax"].toInt(); //int xmax = 512;
    ymin = pars["ymin"].toInt(); //int ymin = 0;
    ymax = pars["ymax"].toInt(); //int ymax = 512;

    segmentMethod = pars["segmentMethod"];

    EW_width = pars["EW_width"].toFloat();

    minADC = pars["minADC"].toInt();
    maxADC = pars["maxADC"].toInt(); // ADC channel value
    ADC_nBins = pars["ADC_nBins"].toInt();
    ADC_cutValue = pars["ADC_cutValue"].toInt();

    minRecE = pars["minRecE"].toInt();
    maxRecE = pars["maxRecE"].toInt(); // keV
    recE_nBins = pars["recE_nBins"].toInt();
    recE_cutValue = pars["recE_cutValue"].toInt();

    crystalNum = nCrystal*nCrystal;
    ADC_binWidth = (maxADC-minADC)/ADC_nBins;
    recE_binWidth = (maxRecE-minRecE)/recE_nBins;

    fName_LUT_P = pars["Position LUT"];
    fName_LUT_E = pars["Energy LUT"];
    fName_LUT_U = pars["Uniformity LUT"];

    fName_LUT_P = currentPath + fName_LUT_P;
    fName_LUT_E = currentPath + fName_LUT_E;
    fName_LUT_U = currentPath + fName_LUT_U;

    for (int i = 0; i < ADC_nBins; ++i)
    {
        ADCs.append(i*ADC_binWidth + ADC_binWidth/2);
    }

    for (int i = 0; i < recE_nBins; ++i)
    {
        recEs.append(i*recE_binWidth + recE_binWidth/2);
    }

    eHistLine = new QLineSeries();
    eHistLine->setPen( QPen(Qt::black, 1) );
    for (int i = 0; i < ADC_nBins; ++i)
    {
        eHistLine->append(ADCs[i], 0);
    }

    EWLeftLine = new QLineSeries();
    EWLeftLine->setPen( QPen(Qt::red, 1));
    EWLeftLine->append(0, 0);
    EWLeftLine->append(0, 1);

    EWRightLine = new QLineSeries();
    EWRightLine->setPen( QPen(Qt::red, 1));
    EWRightLine->append(0, 0);
    EWRightLine->append(0, 1);

    peakLine = new QLineSeries();
    peakLine->setPen( QPen(Qt::red, 1));
    peakLine->append(0, 0);
    peakLine->append(0, 1);

    chart = new QChart();
    chartView = new QChartView();

    axisX = new QValueAxis();
    axisY = new QValueAxis();

    chart->addSeries(eHistLine);
    chart->addSeries(EWLeftLine);
    chart->addSeries(EWRightLine);
    chart->addSeries(peakLine);

    chart->setMargins(QMargins(0, 0, 0, 0));
    chart->legend()->setVisible(false);

    axisX->setRange(0, maxADC);
    axisX->setTickCount(maxADC/10000+1);
    axisX->setLabelFormat("%d");

    axisY->setRange(0, 100);
    axisY->setTickCount(5+1);
    axisY->setLabelFormat("%d");

    chart->addAxis(axisY, Qt::AlignLeft);
    chart->addAxis(axisX, Qt::AlignBottom);

    eHistLine->attachAxis(axisX);
    eHistLine->attachAxis(axisY);

    EWLeftLine->attachAxis(axisX);
    EWLeftLine->attachAxis(axisY);

    EWRightLine->attachAxis(axisX);
    EWRightLine->attachAxis(axisY);

    peakLine->attachAxis(axisX);
    peakLine->attachAxis(axisY);

    chartView->setChart(chart);
    chartView->resize(ui->label_eHist->size());

    QPixmap pixmap = chartView->grab();
    ui->label_eHist->setPixmap(pixmap);

    I0 = cv::Mat_(nPixel, nPixel, 0);
    pt = cv::Mat_(nCrystal, nCrystal, cv::Vec2w(0, 0));
    ShowImage(I0);

    m_eHists = QVector< QVector<quint16> >(crystalNum,
                                          QVector<quint16>(ADC_nBins, 0));
    m_slopes = QVector<double>(crystalNum, 0);

    LogOut("初始化完成。");
}


MainWindow::~MainWindow()
{
    delete ui;
    delete eHistLine;
    delete EWLeftLine;
    delete EWRightLine;
    delete peakLine;
    delete axisX;
    delete axisY;
    delete chart;
    delete chartView;

    delete dataObject;

    LogOut("程序关闭。");

    delete logOutStream;
    logFile->close();
}


// 一些功能性函数
cv::Mat MainWindow::GetColorMap(cv::Mat_<float> I)
{
    cv::Mat_<quint8> scaledMap(I.size(), 0);

    double max_val;
    cv::minMaxLoc(I, NULL, &max_val, NULL, NULL);

    for (int i = 0; i < I.rows; ++i)
    {
        for (int j = 0; j < I.cols; ++j)
        {
            scaledMap(i, j) = std::round(I(i, j)/max_val*255);
        }
    }

    cv::Mat colorMap;
    cv::applyColorMap(scaledMap, colorMap, cv::COLORMAP_HOT);

    return colorMap;
}


void MainWindow::ShowImage(cv::Mat_<float> I)
{
    cv::Mat colorMap = GetColorMap(I);
    QImage qtImage(colorMap.data, colorMap.cols, colorMap.rows,
                   colorMap.step, QImage::Format_BGR888);
    if(qtImage.width()<nPixel || qtImage.height()<nPixel)
    {
        QImage scaledImage = qtImage.scaled(nPixel, nPixel);
        QPixmap pixmap = QPixmap::fromImage(scaledImage);
        ui->label_floodmap->setPixmap(pixmap);
    }
    else
    {
        QPixmap pixmap = QPixmap::fromImage(qtImage);
        ui->label_floodmap->setPixmap(pixmap);
    }
}


void MainWindow::ShowPeaks(cv::Mat_<float> I, cv::Mat_<cv::Vec2w> pt)
{
    cv::Mat colorMap = GetColorMap(I);
    QImage qtImage(colorMap.data, colorMap.cols, colorMap.rows,
                   colorMap.step, QImage::Format_BGR888);
    QPixmap pixmap = QPixmap::fromImage(qtImage);

    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::green, 1));

    for (int i = 0; i < nCrystal; ++i)
    {
        QVector<QPoint> points;
        for (int j = 0; j < nCrystal; ++j)
        {
            int x = pt(j, i)[0];
            int y = pt(j, i)[1];

            int r = 2;
            painter.drawLine(x-r, y+r, x+r, y-r); // "/"
            painter.drawLine(x-r, y-r, x+r, y+r); // "\"
            points.append(QPoint(x, y));
        }
        painter.drawPolyline(points.data(), points.size());
    }

    ui->label_floodmap->setPixmap(pixmap);
}


QVector<int> MainWindow::FindPeaks(QVector<float> v, int nPeaks)
{
    QVector<int> peaks;
    for (int iPeak = 0; iPeak < nPeaks; ++iPeak)
    {
        int maxIdx = std::max_element( v.begin(), v.end() ) - v.begin();
        peaks.append(maxIdx);

        int r = 6;
        int start = std::max(maxIdx-r, 0);
        int stop = std::min(maxIdx+r, nPixel-1);
        for (int i=start; i<=stop; ++i)
        {
            v[i] = 0;
        }
    }

    std::sort(peaks.begin(), peaks.end());
    return peaks;
}


void MainWindow::on_pushButton_readinData_clicked()
{
    LogOut("开始读入数据...");
    QString fName = ui->lineEdit_dataPath->text();
    fName.replace("\\", "/");
    dataObject = new Readin(fName);

    QThread* thread = new QThread(); // 创建新线程
    dataObject->moveToThread(thread);

    QObject::connect(dataObject, SIGNAL(currentPos(int)),
                     this, SLOT(UpdateProgressBar(int)));
    QObject::connect(dataObject, SIGNAL(finished()), this, SLOT(ReStoreData));

    // 启动新线程
    thread->start();

    dataObject->StartReadTxt();
    // dataObject->StartReadBin();
}


void MainWindow::on_pushButton_setEW_clicked()
{
    // 按照 CMID 和 BKID 读入某一个 BK 的数据
    QString CMID = ui->lineEdit_CMID->text();
    QString BKID = ui->lineEdit_BKID->text();

    QString fName = currentPath + "Data/data_CM" + CMID +
                    "_BK" + BKID + ".bin";
    if (!QFile::exists(fName))
    {
        LogOut("文件不存在，请检查文件是否存在。");
        return;
    }

    QFile file(fName);
    if (file.open(QIODevice::ReadOnly))
    {
        quint16 x;
        quint16 y;
        quint16 e;

        m_xList.clear();
        m_yList.clear();
        m_eList.clear();

        QDataStream in(&file);
        while (!in.atEnd())
        {
            in >> x >> y >> e;
            m_xList.append(x);
            m_yList.append(y);
            m_eList.append(e);
        }
        file.close();
    }
    else
    {
        LogOut("文件打开失败，请检查文件是否正确。");
        return;
    }

    // 产生能谱, histogram
    QVector<int> h0(ADC_nBins, 0);
    for (const auto& e : m_eList)
    {
        if(0<e && e<maxADC)
        {
            int idx = e/ADC_binWidth;
            h0[idx]++;
        }
    }

    QVector<float> h1 = Smooth(h0);

    float peak_x = 0;
    m_peakValue = 0;
    QVector<QPointF> points;
    for (int i = 0; i < ADC_nBins; ++i)
    {
        float x = ADCs[i];
        float y = h1[i];
        points.append(QPointF(x, y));

        if (y > m_peakValue)
        {
            peak_x = x;
            m_peakValue = y;
        }
    }

    eHistLine->replace(points);

    int minEW = std::round(peak_x*(1-EW_width));
    int maxEW = std::round(peak_x*(1+EW_width));

    axisX->setRange(0, maxADC);
    axisY->setRange(0, m_peakValue*1.1);

    EWLeftLine->replace(0, minEW, 0);
    EWLeftLine->replace(1, minEW, m_peakValue);

    EWRightLine->replace(0, maxEW, 0);
    EWRightLine->replace(1, maxEW, m_peakValue);

    QPixmap pixmap = chartView->grab();
    ui->label_eHist->setPixmap(pixmap);

    ui->lineEdit_minEW->setText(QString::number(minEW));
    ui->lineEdit_maxEW->setText(QString::number(maxEW));
    LogOut("画出整个BK能谱，并自动生成峰值左右各25%能窗参数。");

    // 参照能窗的设置参数，筛选数据
    I0 = cv::Mat::zeros(nPixel, nPixel, CV_32FC1);
    for (int i = 0; i < m_eList.size(); ++i)
    {
        int x = m_xList[i];
        int y = m_yList[i];
        int e = m_eList[i];

        if(minEW<e && e<maxEW && xmin<x && x<xmax && ymin<y && y<ymax)
        {
            I0(y, x) += 1;
        }
    }
    ShowImage(I0);
    //LogOut("Generate I0. ");

    //cv::Mat color_I0 = GetColorMap(I0);
    //cv::namedWindow("I0", cv::WINDOW_NORMAL); // cv::WINDOW_AUTOSIZE
    //cv::imshow("I0", color_I0);
}


void MainWindow::on_pushButton_segment_clicked()
{
    LogOut("开始分割...");
    imgFlag = 0;

    if(segmentMethod=="SVD")
    {
        // 自动寻峰, 奇异值分解, 寻找规则峰位作为平均值移动算法的迭代初始位置
        // 平均值移动算法，寻找peaks
        cv::Mat U, S, Vt;
        cv::SVD::compute(I0, S, U, Vt); // 进行奇异值分解

        cv::Mat S1 = cv::Mat::zeros(I0.size(), CV_32FC1);
        S1.at<float>(0, 0) = S.at<float>(0, 0);
        cv::Mat I1 = U*S1*Vt;
        LogOut("SVD done. ");

        cv::Mat col_sum, row_sum;
        cv::reduce(I1, col_sum, 0, cv::REDUCE_SUM);
        cv::reduce(I1, row_sum, 1, cv::REDUCE_SUM);

        QVector<float> colSumVec =
            {col_sum.begin<float>(), col_sum.end<float>()};
        QVector<float> rowSumVec =
            {row_sum.begin<float>(), row_sum.end<float>()};
        QVector<int> peak_x_index = FindPeaks(colSumVec, nCrystal);
        QVector<int> peak_y_index = FindPeaks(rowSumVec, nCrystal);

        for (int i = 0; i < nCrystal; ++i)
        {
            for (int  j= 0; j < nCrystal; ++j)
            {
                pt(i, j)[0] = peak_x_index[j];
                pt(i, j)[1] = peak_y_index[i];
            }
        }

        LogOut("所有晶体初始位置设置完毕。");

        // step1，每组整体做平均值移动
        for (int iRow = 0; iRow < num1; ++iRow)
        {
            for (int iCol = 0; iCol < num1; ++iCol)
            {
                // 求 4 个点的重心的初始位置
                cv::Point2f c0(0, 0);
                for (int i = 0; i < num2; ++i)
                {
                    for (int j = 0; j < num2; ++j)
                    {
                        c0 += cv::Point2f(pt(iRow*num2+i, iCol*num2+j)[0],
                                          pt(iRow*num2+i, iCol*num2+j)[1]);
                    }
                }

                cv::Point2f c = c0/(num2*num2); // 初始位置
                cv::Point2f pre_c(0, 0);

                //int counter = 0;
                while (cv::norm(pre_c-c) > 1)  // 迭代移动重心
                {
                    //counter++;
                    pre_c = c;
                    c = cv::Point2f(0, 0);

                    // 计算重心
                    for (int i = 0; i < num2; ++i)
                    {
                        for (int j = 0; j < num2; ++j)
                        {
                            cv::Point2f x(pt(iRow*num2+i, iCol*num2+j)[0],
                                          pt(iRow*num2+i, iCol*num2+j)[1]);

                            // 累加项
                            cv::Point2f u(0, 0);
                            float d = 0;
                            int r = 6;
                            for (int m = x.x-r; m <= x.x+r; ++m)
                            {
                                for (int n = x.y-r; n <= x.y+r; ++n)
                                {
                                    if (cv::norm(x-cv::Point2f(m, n)) <= r)
                                    {
                                        u += I0(n, m)*cv::Point2f(m, n);
                                        d += I0(n, m);
                                    }
                                }
                            }

                            if(d==0)
                            {
                                LogOut( "迭代过程中，分母 d = 0 !!! Bug !!!" );
                            }
                            else
                            {
                                c += u/d;
                            }
                        }
                    }

                    c = c/(num2*num2);

                    for (int i = 0; i < num2; ++i)
                    {
                        for (int j = 0; j < num2; ++j)
                        {
                            pt(iRow*num2+i, iCol*num2+j)[0] +=
                                std::round(c.x - pre_c.x);
                            pt(iRow*num2+i, iCol*num2+j)[1] +=
                                std::round(c.y - pre_c.y);
                        }
                    }
                }
                //qDebug() << "The first mean-shift times is " << counter;
            }
        }
        LogOut("第一次平均值移动迭代，每组峰位移动完成。");

        // step2, all peak 做平均值移动
        //循环遍历所有峰，使得每个峰做小范围移动
        for (int iRow = 0; iRow < nCrystal; ++iRow)
        {
            for (int iCol = 0; iCol < nCrystal; ++iCol)
            {
                cv::Point2f x(pt(iRow, iCol)[0], pt(iRow, iCol)[1]);
                cv::Point2f pre_x(0, 0);
                //int counter = 0;
                while(cv::norm(pre_x-x)>0.1)
                {
                    //counter++;
                    pre_x = x;
                    cv::Point2f u(0, 0);
                    float d = 0;

                    int r = 5;
                    for (int i = x.x-r; i <= x.x+r; ++i)
                    {
                        for (int j = x.y-r; j <= x.y+r; ++j)
                        {
                            if (cv::norm(x-cv::Point2f(i, j)) < r)
                            {
                                float k =
                                    cv::norm(x-cv::Point2f(i, j))/pow((2*r+1), 2);
                                float g = (1/sqrt(2*M_PI))*exp(-0.5*k);
                                u += I0(j, i)*g*cv::Point2f(i, j);
                                d += I0(j, i)*g;
                            }
                        }
                    }

                    if (d==0)
                    {
                        LogOut( "迭代过程中，分母 d = 0 !!! Bug !!!" );
                    }
                    else
                    {
                        x = u/d;
                    }
                }
                //qDebug() << "The second mean-shift times is " << counter;

                pt(iRow, iCol)[0] = std::round(x.x);
                pt(iRow, iCol)[1] = std::round(x.y);
            }
        }
        LogOut("第二次平均值移动迭代。所有峰位寻找完成。");
    }


    if(segmentMethod=="FindMaximum") // maximum method to find peaks
    {
        // 使用 OpenCV 的平滑函数对数据进行平滑处理
        cv::Mat_<float> I1;
        cv::blur(I0, I1, cv::Size(3, 3));

        //cv::Mat_<float> I2;
        //cv::GaussianBlur(I1, I2, cv::Size(3, 3), 1, 1);
        //I1 = I2;

        QVector<cv::Point> peaks;
        //for (int i = 0; i < crystalNum; ++i)
        for (int i = 0; i < 20; ++i)
        {
            double minVal, maxVal;
            cv::Point minLoc, maxLoc;
            cv::minMaxLoc(I1, &minVal, &maxVal, &minLoc, &maxLoc);

            peaks.append(maxLoc);

            // 将maxLoc附近的元素置为0
            int m0 = maxLoc.x;
            int n0 = maxLoc.y;
            int r = 5;

            for (int n = n0-r; n <= n0+r; ++n)
            {
                for (int m = m0-r; m <=m0+r ; ++m)
                {
                    if(cv::norm(maxLoc-cv::Point(m, n))<r)
                    {
                        I1(n, m) = 0;
                    }
                }
            }
        }

        // sort peaks
        for (int iRow = 0; iRow < nCrystal; ++iRow)
        {
            // 选出 y 值较小的 nCrystal 个点作为一行， y值升序排列
            std::sort(peaks.begin(), peaks.end(),
                      [](const cv::Point& p1, const cv::Point& p2){return p1.y < p2.y;});

            QVector<cv::Point> rowPeaks(peaks.begin(), peaks.begin()+nCrystal);
            // 将他们按 x 值升序排列
            std::sort(rowPeaks.begin(), rowPeaks.end(),
                      [](const cv::Point& p1, const cv::Point& p2){return p1.x < p2.x;});

            for (int iCol = 0; iCol < nCrystal; ++iCol)
            {
                peaks.pop_front();
                pt(iRow, iCol)[0] = rowPeaks[iCol].x;
                pt(iRow, iCol)[1] = rowPeaks[iCol].y;
            }
        }
    }

    ShowPeaks(I0, pt);
}


// 此函数生成整机可用的位置查找表，即将各个BK的分割结果汇总起来变成一个文件
void MainWindow::on_pushButton_genPositionLUT_clicked()
{
    //根据分割结果，生成位置查找表
    QFile file(fName_LUT_P);
    if(!file.open(QIODevice::WriteOnly))
    {
        LogOut("打开文件失败，无法生成位置查找表。");
        return;
    }

    QDataStream out(&file);

    for (int iCM = 0; iCM < nCM; ++iCM)
    {
        for (int iBK = 0; iBK < nBK; ++iBK)
        {
            QString fName = currentPath + "Data/seg_CM" + QString::number(iCM) +
                            "_BK" + QString::number(iBK) + ".xml";
            cv::FileStorage fs(fName.toStdString(), cv::FileStorage::READ);
            if (!fs.isOpened())
            {
                LogOut( "无法打开文件: " + fName );
                return;
            }

            cv::Mat_<quint16> segr;
            fs["segr"] >> segr; // 读取矩阵数据
            fs.release(); // 关闭文件

            // 写入矩阵数据
            for (int iRow = 0; iRow < nPixel; ++iRow)
            {
                for (int iCol = 0; iCol < nPixel; ++iCol)
                {
                    out << segr(iRow, iCol);
                }
            }
        }
    }

    file.close();
    LogOut( "位置查找表已生成: " + fName_LUT_P );
}


void MainWindow::on_pushButton_genEnergyLUT_clicked()
{
    double peakE = ui->lineEdit_peakEnergy->text().toDouble();

    QFile outFile(fName_LUT_E);
    if(!outFile.open(QIODevice::WriteOnly))
    {
        LogOut("打开文件失败，无法生成能量查找表。");
        return;
    }
    QDataStream outStream(&outFile);


    for (int iCM = 0; iCM < nCM; ++iCM)
    {
        for (int iBK = 0; iBK < nBK; ++iBK)
        { 
            QString CMID = QString::number(iCM);
            QString BKID = QString::number(iBK);

            // 读入分割结果
            QString fName = currentPath + "Data/seg_CM" + CMID + "_BK" + BKID + ".xml";
            cv::FileStorage fs(fName.toStdString(), cv::FileStorage::READ);
            cv::Mat_<quint16> segr;
            fs["segr"] >> segr;
            fs.release();

            // 读入原始数据
            fName = currentPath + "Data/data_CM" + CMID + "_BK" + BKID + ".bin";
            QFile inFile(fName);
            if (!inFile.open(QIODevice::ReadOnly))
            {
                LogOut("打开原始数据文件失败，无法生成能量查找表。");
                return;
            }

            QDataStream in(&inFile);

            quint16 x;
            quint16 y;
            quint16 e;

            m_eHists = QVector< QVector<quint16> >(crystalNum,
                                                  QVector<quint16>(ADC_nBins, 0));
            while (!in.atEnd())
            {
                in >> x >> y >> e;
                if(xmin<x && x<xmax && ymin<y && y<ymax && minADC<e && e<maxADC)
                {
                    int crystalID = segr(y, x);
                    int idx = e/ADC_binWidth;
                    m_eHists[crystalID][idx]++;
                }
            }
            inFile.close();

            // 将能谱也存成文件，以便修改峰位时调用
            fName = currentPath + "Data/eHist_CM" + CMID + "_BK" + BKID + ".bin";
            QFile eHist_outFile(fName);
            if(!eHist_outFile.open(QIODevice::WriteOnly))
            {
                LogOut("无法保存能谱文件。");
            }
            QDataStream outEHist(&eHist_outFile);

            m_slopes.clear();
            for (int i = 0; i < crystalNum; ++i)
            {
                auto h0 = m_eHists[i];
                QVector<float> h1 = Smooth(h0);
                QVector<float> h2 = Smooth(h1);

                for (const auto& var : h2)
                {
                    outEHist << quint16(var);  // 保存能谱
                }

                int NN = ADC_cutValue/ADC_binWidth; // 将0--NN元素置为0，防止寻峰错误
                for (int iBin = 0; iBin < NN; ++iBin)
                {
                    h2[iBin] = 0;
                }

                int idx = std::max_element(h2.begin(), h2.end()) - h2.begin();
                double aSlope = peakE/ADCs[idx];
                m_slopes.push_back(aSlope);
                outStream << aSlope;
            }

            eHist_outFile.close();
        }
    }

    outFile.close();
    LogOut("能量查找表已生成: " + fName_LUT_E);
}


void MainWindow::on_pushButton_calPeaks_clicked()
{
    // 调取能谱数据，查看单根晶体刻度过程中，自动寻峰结果是否完全正确
    // 如果不正确可以修改，并重新生成能量查找表
    imgFlag = 1;

    // 打开[CMID, BKID]指定的 BK 的能谱文件
    QString CMID = ui->lineEdit_CMID->text();
    QString BKID = ui->lineEdit_BKID->text();

    // 读入能谱数据
    QString fName = currentPath + "Data/eHist_CM" + CMID + "_BK" + BKID + ".bin";
    QFile file(fName);
    if (!file.open(QIODevice::ReadOnly))
    {
        LogOut("打开能谱文件失败。");
        return;
    }
    QDataStream in(&file);

    m_eHists.clear();
    for (int i = 0; i < crystalNum; ++i)
    {
        QVector<quint16> iHist(ADC_nBins, 0);
        for (int iBin = 0; iBin < ADC_nBins; ++iBin)
        {
            in >> iHist[iBin];
        }
        m_eHists.append(iHist);
    }
    file.close();

    EWLeftLine->replace(0, 0, 0);
    EWLeftLine->replace(1, 0, 0);

    EWRightLine->replace(0, 0, 0);
    EWRightLine->replace(1, 0, 0);

    chartView->resize(nPixel, 400);
    axisX->setRange(minADC, maxADC);

    // 读入能量查找表
    QFile infile(fName_LUT_E);
    if(infile.open(QIODevice::ReadOnly))
    {
        QDataStream in(&infile);
        m_slopes.clear();
        for (int i = 0; i < crystalNum; ++i)
        {
            float slope;
            in >> slope;
            m_slopes.append(slope);
        }
        infile.close();
    }
    else
    {
        LogOut("打开能量查找表文件失败。");
        return;
    }

    ShowEHist();
}


void MainWindow::ShowEHist(int peakLoc)
{
    double peakE = ui->lineEdit_peakEnergy->text().toDouble();

    int m = ui->lineEdit_colID->text().toInt();
    int n = ui->lineEdit_rowID->text().toInt();
    int crystalID = n*nCrystal + m;

    QVector<quint16> h0 = m_eHists[crystalID];

    int x = int(peakE/m_slopes[crystalID]);
    float y = *std::max_element(h0.begin(), h0.end());

    QVector<QPointF> points;
    for (int i = 0; i < ADC_nBins; ++i)
    {
        points.append(QPointF(ADCs[i], h0[i]));
    }
    eHistLine->replace(points);

    if(-1==peakLoc)
    {
        peakLine->replace(0, x, 0);
        peakLine->replace(1, x, y);
    }
    else
    {
        peakLine->replace(0, peakLoc, 0);
        peakLine->replace(1, peakLoc, y);
    }

    axisY->setRange(0, y*1.1);

    QPixmap pixmap = chartView->grab();
    ui->label_floodmap->setPixmap(pixmap);
}


void MainWindow::on_pushButton_calOnePeak_clicked()
{
    // 只读取能谱文件中的某一段数据，即某个晶体的能谱数据
    imgFlag = 2;
    float peakE = ui->lineEdit_peakEnergy->text().toDouble();

    //按照界面中设置的行号和列号，读入该晶体的能谱
    int CMID = ui->lineEdit_CMID->text().toInt();
    int BKID = ui->lineEdit_BKID->text().toInt();

    int rowID = ui->lineEdit_rowID->text().toInt();
    int colID = ui->lineEdit_colID->text().toInt();
    int crystalID = rowID*nCrystal + colID;
    qDebug() << "crystal ID = " << crystalID;

    // 打开[CMID, BKID]指定的 BK 的能谱文件
    QString fName = currentPath + "Data/eHist_CM" + QString::number(CMID) +
                    "_BK" + QString::number(BKID) + ".bin";
    QFile infile(fName);
    if(infile.open(QIODevice::ReadOnly))
    {
        QDataStream in(&infile);
        infile.seek(crystalID*ADC_nBins*sizeof(quint16)); // 设置读取位置，然后读入
        for (int iBin = 0; iBin < ADC_nBins; ++iBin)
        {
            in >> m_eHists[crystalID][iBin];
        }
        infile.close();
    }
    else
    {
        LogOut("打开能谱文件失败。");
    }

    // 读入之前确定的能峰峰位
    QFile peaksFile(fName_LUT_E);
    int peakLoc = 0;
    if(peaksFile.open(QIODevice::ReadOnly))
    {
        QDataStream in(&peaksFile);
        int totalBKID = CMID*nBK + BKID;
        int position = (totalBKID*crystalNum+crystalID)*sizeof(double);
        peaksFile.seek(position);
        float slope;
        in >> slope;
        peakLoc = std::round(peakE/slope);
        peaksFile.close();
    }
    else
    {
        LogOut("打开能量查找表文件失败。");
    }

    // 将能谱和能峰画图
    auto h0 = m_eHists[crystalID];

    QVector<QPointF> points;
    for (int i = 0; i < ADC_nBins; ++i)
    {
        points.append(QPointF(ADCs[i], h0[i]));
    }

    eHistLine->replace(points);

    float y = *std::max_element(h0.begin(), h0.end());
    peakLine->replace(0, peakLoc, 0);
    peakLine->replace(1, peakLoc, y);

    axisY->setRange(0, y*1.1);

    QPixmap pixmap = chartView->grab();
    ui->label_floodmap->setPixmap(pixmap);
}


void MainWindow::on_label_eHist_mouseLeftClicked()
{
    // 将点击位置从窗口坐标系转换到图表坐标系
    QPoint leftPos = ui->label_eHist->GetLeftPos();
    auto coordinate = chart->mapToValue(leftPos);
    int minEW = int(coordinate.x());

    EWLeftLine->replace(0, minEW, 0);
    EWLeftLine->replace(1, minEW, m_peakValue);

    QPixmap pixmap = chartView->grab();
    ui->label_eHist->setPixmap(pixmap);

    ui->lineEdit_minEW->setText(QString::number(minEW));
}


void MainWindow::on_label_eHist_mouseRightClicked()
{
    // 将点击位置从窗口坐标系转换到图表坐标系
    QPoint rightPos = ui->label_eHist->GetRightPos();
    auto coordinate = chart->mapToValue(rightPos);
    int maxEW = int(coordinate.x());

    EWRightLine->replace(0, maxEW, 0);
    EWRightLine->replace(1, maxEW, m_peakValue);

    QPixmap pixmap = chartView->grab();
    ui->label_eHist->setPixmap(pixmap);

    ui->lineEdit_maxEW->setText(QString::number(maxEW));
}


void MainWindow::on_label_floodmap_mouseLeftClicked()
{
    if(0==imgFlag)
    {
        // step 3, 手动修正 peak 位置
        QPoint pos = ui->label_floodmap->GetPos();
        cv::Mat_<float> dis(nCrystal, nCrystal, 0.0);
        for (int i = 0; i < nCrystal; ++i)
        {
            for (int j = 0; j < nCrystal; ++j)
            {
                cv::Point2f p1(pt(i, j)[0], pt(i, j)[1]);
                cv::Point2f p2(pos.x(), pos.y());
                dis(i, j) = cv::norm(p1-p2);
            }
        }

        cv::Point min_loc;
        cv::minMaxLoc(dis, NULL, NULL, &min_loc, NULL);

        pt(min_loc)[0] = pos.x();
        pt(min_loc)[1] = pos.y();

        ShowPeaks(I0, pt);
    }

    if(1==imgFlag || 2==imgFlag)
    {
        QPoint pos = ui->label_floodmap->GetPos();
        auto coordinate = chart->mapToValue(pos);
        quint16 peakLoc = quint16(coordinate.x());

        int rowID = ui->lineEdit_rowID->text().toInt();
        int colID = ui->lineEdit_colID->text().toInt();
        int crystalID = rowID*nCrystal + colID;
        double peakE = ui->lineEdit_peakEnergy->text().toDouble();
        m_slopes[crystalID] = peakE/peakLoc;
        ShowEHist(peakLoc);
    }
}


void MainWindow::on_label_floodmap_mouseRightClicked()
{
    if(0==imgFlag)
    {
        //step 4, 开始分割, //计算像素点属于哪个峰
        cv::Mat_<quint16> seg1(nPixel, nPixel); // 列信息
        cv::Mat_<quint16> seg2(nPixel, nPixel); // 行信息

        for (int i = 0; i < nPixel; ++i)
        {
            for (int j = 0; j < nPixel; ++j)
            {
                cv::Mat_<float> dis(nCrystal, nCrystal);
                for (int m = 0; m < nCrystal; ++m)
                {
                    for (int n = 0; n < nCrystal; ++n)
                    {
                        cv::Point2f p(pt(m, n)[0], pt(m, n)[1]);
                        dis(m, n) = cv::norm(p-cv::Point2f(i, j));
                    }
                }

                cv::Point min_loc;
                cv::minMaxLoc(dis, NULL, NULL, &min_loc, NULL);
                seg1(i, j) = min_loc.x;
                seg2(i, j) = min_loc.y;
            }
        }

        cv::Mat_<quint16> segr = seg2*nCrystal + seg1;
        cv::Mat_<quint8> edge = cv::Mat::zeros(nPixel, nPixel, CV_8UC1);
        for (int i = 1; i < nPixel-1; ++i)
        {
            for (int j = 1; j < nPixel-1; ++j)
            {
                int id0 = segr(i, j);
                int id1 = segr(i+1, j);
                int id2 = segr(i, j+1);
                if(id0!=id1 || id0!=id2)
                {
                    edge(i, j) = 1;
                }
            }
        }

        double max_val;
        cv::minMaxLoc(I0, NULL, &max_val, NULL, NULL);

        cv::Mat_<float> I4 = I0.clone();
        for (int i = 0; i < nPixel; ++i)
        {
            for (int j = 0; j < nPixel; ++j)
            {
                if( edge(i, j) == 1 )
                {
                    I4(i, j) = max_val*1.1;
                }
            }
        }

        // 将每个BK 的分割结果保存成文件，以便后面调用
        // 这样做的必要性是因为所有BK分割完才能生成整台机器的位置查找表
        // 这样做还有一个优点，如果某个BK分割结果不理想，
        // 后期还可以单独分割这个BK，然后生成位置查找表
        QString CMID = ui->lineEdit_CMID->text();
        QString BKID = ui->lineEdit_BKID->text();
        QString fileName = "./Data/seg_CM" + CMID +  "_BK" + BKID + ".xml";

        // 创建文件存储对象
        cv::FileStorage fs(fileName.toStdString(), cv::FileStorage::WRITE);
        fs.write("floodmap", I0); // 将I0 也写入
        fs.write("edge", edge);   // 写入边界结果
        fs.write("segr", segr);   // 写入矩阵数据
        fs.release();  // 关闭文件

        ShowImage(I4);

        // 保存分割结果图片
        QPixmap pixmap = ui->label_floodmap->pixmap();
        QImage image = pixmap.toImage();
        QString figName = "./Data/seg_CM" + CMID +  "_BK" + BKID + ".png";
        if ( image.save(figName) )
        {
            qDebug() << "Image saved successfully to" << figName;
        }
        else
        {
            qDebug() << "Failed to save image to" << figName;
        }

        QMessageBox::information(this, "分割过程", "分割完毕，位置查找表已生成。");
    }

    if(1==imgFlag)
    {
        int m = ui->lineEdit_colID->text().toInt();
        int n = ui->lineEdit_rowID->text().toInt();

        if(n==(nCrystal-1) && m==(nCrystal-1))
        {
            return;
        }

        if(m==nCrystal-1)
        {
            n++;
            m = 0;
        }
        else
        {
            m++;
        }

        ui->lineEdit_colID->setText(QString::number(m));
        ui->lineEdit_rowID->setText(QString::number(n));

        ShowEHist();
    }

    if(2==imgFlag)
    {
        // nothing to do!
    }
}


void MainWindow::on_pushButton_calSegFOM_clicked()
{
    // step 5, 求分割质量参数
    // 读入 I0、segr
    QString CMID = ui->lineEdit_CMID->text();
    QString BKID = ui->lineEdit_BKID->text();
    QString segrFile = "./Data/seg_CM" + CMID + "_BK" + BKID + ".xml";
    cv::FileStorage fs(segrFile.toStdString(), cv::FileStorage::READ);

    cv::Mat_<quint16> segr;
    cv::Mat_<quint8> edge;

    fs["floodmap"] >> I0;
    fs["edge"] >> edge;
    fs["segr"] >> segr;

    fs.release();

    // IR 是图像均值比去分割线上的计数，约大质量越好
    // rms 是各个分割区域的均方差，越小越好
    quint64 totalEvts = 0;
    quint64 edgeEvts = 0;
    quint64 edgeN = 0;
    QVector< QVector<int> > nEvts(crystalNum);
    int pixelCounter = 0;
    for (int i = 0; i < nPixel; ++i)
    {
        for (int j = 0; j < nPixel; ++j)
        {
            if ( ymin<i && i<ymax && xmin<j && j<xmax )
            {
                float aN = I0(i, j);
                totalEvts += aN;
                pixelCounter++;

                if ( 1==edge(i, j) )
                {
                    edgeN++;
                    edgeEvts += aN;
                }

                int crystalID = segr(i, j);
                nEvts[crystalID].append(aN);
            }
        }
    }

    float IR = 1.0*totalEvts/pixelCounter/(1.0*edgeEvts/edgeN);
    ui->lineEdit_IR->setText(QString::number(IR, 'f', 2));

    QVector<float> RMS;
    for (int iCrystal=0; iCrystal<crystalNum; ++iCrystal)
    {
        // 求每个晶体的rms
        QVector<int> data = nEvts[iCrystal];
        int sum = std::accumulate(data.begin(), data.end(), 0);
        float mean = 1.0*sum/data.size();

        // 计算每个数据点与平均值的差值的平方和
        float sumSquaredDiff = 0.0;
        for (auto value : data)
        {
            float diff = value - mean;
            sumSquaredDiff += diff * diff;
        }

        // 求平方和的平均值
        float meanSquaredDiff = sumSquaredDiff / data.size();

        // 取平方根，即均方差
        float aRMS = sqrt(meanSquaredDiff);
        RMS.append(aRMS);
    }

    // 求所有晶体的平均RMS
    float meanRMS = std::accumulate(RMS.begin(), RMS.end(), 0.0)/RMS.size();
    ui->lineEdit_RMS->setText(QString::number(meanRMS, 'f', 2));
}


// 计算移动平均值
template<typename T>
QVector<float> MainWindow::Smooth(const QVector<T>& data, int window)
{
    // 将 std::vector 转换为 cv::Mat
    cv::Mat dataMat(1, data.size(), CV_32F);
    for (size_t i = 0; i < data.size(); ++i)
    {
        dataMat.at<float>(0, i) = data[i];
    }

    // 使用 OpenCV 的平滑函数对数据进行平滑处理
    cv::Mat smoothedDataMat;
    cv::blur(dataMat, smoothedDataMat, cv::Size(window, 1));

    // 将平滑后的数据转换为 std::vector
    QVector<float> smoothedData(smoothedDataMat.begin<float>(),
                                smoothedDataMat.end<float>());
    return smoothedData;
}


void MainWindow::on_pushButton_writePeaks_clicked()
{
    QFile file(fName_LUT_E);
    if (!file.open(QIODevice::WriteOnly))
    {
        LogOut("文件打开失败，无法生成能量查找表。");
        return;
    }

    QDataStream out(&file);
    for (const auto& var : m_slopes)
    {
        out << var;
    }
    file.close();
}


void MainWindow::on_pushButton_calEnergyResolution_clicked()
{
    // 打开[CMID, BKID]指定的 BK 的能谱文件, 读入E Hist数据
    QString CMID = ui->lineEdit_CMID->text();
    QString BKID = ui->lineEdit_BKID->text();

    // 读入能量校正表
    QVector<float> ADC2keV(crystalNum, 0);
    QFile infile(fName_LUT_E);
    if (infile.open(QIODevice::ReadOnly))
    {
        QDataStream in(&infile);
        for (int i = 0; i < crystalNum; ++i)
        {
            in >> ADC2keV[i];
        }
        infile.close();
    }
    else
    {
        LogOut("文件打开失败，无法读入能量查找表。");
    }

    // 读入分割结果
    QString fName = currentPath + "Data/seg_CM" + CMID + "_BK" + BKID + ".xml";
    cv::FileStorage fs(fName.toStdString(), cv::FileStorage::READ);
    cv::Mat_<quint16> segr;
    fs["segr"] >> segr;
    fs.release();

    QVector<int> totalEHist(recE_nBins, 0);
    QVector< QVector<int> > eHists(crystalNum, QVector<int>(recE_nBins, 0));

    // 读入原始数据
    fName = currentPath + "Data/data_CM" + CMID + "_BK" + BKID + ".bin";
    QFile inFile(fName);
    if (inFile.open(QIODevice::ReadOnly))
    {
        QDataStream in(&inFile);

        quint16 x;
        quint16 y;
        quint16 e;

        while (!in.atEnd())
        {
            in >> x >> y >> e;
            if(xmin<x && x<xmax && ymin<y && y<ymax && minADC<e && e<maxADC)
            {
                int crystalID = segr(y, x);
                float slope = ADC2keV[crystalID];
                float e_keV = e*slope; // ADC==>keV
                if(minRecE<e_keV && e_keV<maxRecE)
                {
                    int idx = std::floor(e_keV/recE_binWidth);
                    totalEHist[idx]++;
                    eHists[crystalID][idx]++;
                }
            }
        }
        inFile.close();
    }
    else
    {
        LogOut("打开原始数据文件失败，无法生成能量查找表。");
        return;
    }

    // 计算单根分辨率
    QVector<float> ERs;
    for (int i = 0; i < crystalNum; ++i)
    {
        auto iHist = eHists[i];
        float aER =  GetER(iHist);
        ERs.append(aER);
    }

    // 画单根分辨率map 在label_floodmap
    cv::Mat_<float> ERMat(nCrystal, nCrystal);
    for (int iRow = 0; iRow < nCrystal; ++iRow)
    {
        for (int iCol = 0; iCol < nCrystal; ++iCol)
        {
            float aER = ERs[iRow*nCrystal + iCol];
            ERMat(iRow, iCol) = aER;
            //qDebug() << "n = " << iRow << ", m = " << iCol << ", ER = " << aER;
        }
    }

    ShowImage(ERMat);

    // 计算总分辨率
    // 寻找峰值和峰位
    int peakIndex = std::max_element( totalEHist.begin(),
                                     totalEHist.end() ) - totalEHist.begin();
    int peakValue = totalEHist[peakIndex];

    // 计算半高宽
    float halfMax = 1.0*peakValue / 2;
    int leftIndex = peakIndex;
    int rightIndex = peakIndex;

    while (leftIndex > 0 && totalEHist[leftIndex-1]>halfMax)
    {
        leftIndex--;
    }
    while (rightIndex < totalEHist.size()-1 && totalEHist[rightIndex+1]>halfMax)
    {
        rightIndex++;
    }

    float peakLocValue = recEs[peakIndex];
    float leftValue = recEs[leftIndex];
    float rightValue = recEs[rightIndex];
    float FWHM = rightValue - leftValue; // 半高宽

    qDebug() << "peakLocValue = " << peakLocValue;
    qDebug() << "leftValue = " << leftValue;
    qDebug() << "rightValue = " << rightValue;
    qDebug() << "FWHM = " << FWHM;

    // 计算能量分辨率
    float totalER = 100.0 * FWHM / peakLocValue;

    // 画总能谱在 label_ehist
    QVector<QPointF> points;
    for (int i = 0; i < recE_nBins; ++i)
    {
        float x = recEs[i];
        float y = totalEHist[i];
        points.append( QPointF(x, y) );
    }

    eHistLine->replace(points);

    EWLeftLine->replace(0, leftValue, 0);
    EWLeftLine->replace(1, leftValue, peakValue);

    EWRightLine->replace(0, rightValue, 0);
    EWRightLine->replace(1, rightValue, peakValue);

    axisX->setRange(minRecE, maxRecE);
    axisY->setRange(0, std::round(peakValue*1.1));
    chartView->resize(ui->label_eHist->size());

    QPixmap pixmap = chartView->grab();
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::red, 1));

    // 如果想画半高线，需要把把坐标从chart 转换成 charview中的像素坐标
    //painter.setPen(QPen(Qt::red, 1));
    //painter.drawLine( leftIndex, std::round(halfMax),
    //                 rightIndex, std::round(halfMax) );

    painter.setFont(QFont("Arial", 12));
    painter.drawText(QPoint(300, 100), QString::number(totalER, 'f', 2) + " %");

    ui->label_eHist->setPixmap(pixmap);
    LogOut("total energy resolution is " + QString::number(totalER, 'f', 2) + " %.");
}


float MainWindow::GetER(QVector<int>& y) // return %
{
    // 寻找峰值和峰位
    int peakIndex = std::max_element(y.begin(), y.end()) - y.begin();
    int peakValue = y[peakIndex];

    // 计算半高宽
    float halfMax = 1.0*peakValue / 2;
    int leftIndex = peakIndex;
    int rightIndex = peakIndex;
    while (leftIndex > 0 && y[leftIndex-1] >= halfMax)
    {
        leftIndex--;
    }
    while (rightIndex < y.size()-1 && y[rightIndex+1] >= halfMax)
    {
        rightIndex++;
    }
    int FWHM = rightIndex - leftIndex + 1; // 半高宽

    // 计算能量分辨率
    float ER = 100.0* FWHM / peakIndex;
    return ER;
}


void MainWindow::on_pushButton_calUniformity_clicked()
{
    cv::Mat_<float> nEvts = cv::Mat::zeros(nCrystal, nCrystal, CV_32FC1);

    int totalEvts = 0;

    // 读入能谱数据
    QString CMID = ui->lineEdit_CMID->text();
    QString BKID = ui->lineEdit_BKID->text();
    QString fName = currentPath + "Data/eHist_CM" + CMID + "_BK" + BKID + ".bin";
    QFile file(fName);
    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream in(&file);

        for (int iCrystal = 0; iCrystal < crystalNum; ++iCrystal)
        {
            QVector<quint16> iHist(ADC_nBins, 0);
            for (int iBin = 0; iBin < ADC_nBins; ++iBin)
            {
                in >> iHist[iBin];
            }

            int sum = std::accumulate(iHist.begin(), iHist.end(), 0);
            totalEvts += sum;

            int iRow = iCrystal / nCrystal;
            int iCol = iCrystal % nCrystal;

            nEvts(iRow, iCol) = sum;
        }

        file.close();
    }
    else
    {
        LogOut("Failed to open the ehist file.");
    }

    ShowImage(nEvts);

    float mean_nEvts = 1.0*totalEvts/(crystalNum);
    cv::Mat_<float> uniformityPar = mean_nEvts/nEvts;

    QFile outfile(fName_LUT_U);
    if ( outfile.open(QIODevice::WriteOnly) )
    {
        QDataStream out(&outfile);

        for (int iRow = 0; iRow < nCrystal; ++iRow)
        {
            for (int iCol = 0; iCol < nCrystal; ++iCol)
            {
                float c = uniformityPar(iRow, iCol);
                out << c;
            }
        }
        outfile.close();
    }
    else
    {
        LogOut("均匀性查找表生成失败。");
    }
}


void MainWindow::on_lineEdit_minEW_editingFinished()
{
    int minEW = ui->label_minEW->text().toInt();
    EWLeftLine->replace(0, minEW, 0);
    EWLeftLine->replace(1, minEW, m_peakValue);

    QPixmap pixmap = chartView->grab();
    ui->label_eHist->setPixmap(pixmap);
}


void MainWindow::on_lineEdit_maxEW_editingFinished()
{
    int maxEW = ui->label_maxEW->text().toInt();
    EWRightLine->replace(0, maxEW, 0);
    EWRightLine->replace(1, maxEW, m_peakValue);

    QPixmap pixmap = chartView->grab();
    ui->label_eHist->setPixmap(pixmap);
}


void MainWindow::LogOut(QString str)
{
    QString s0 = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    QString s1 = s0 + "  " + str;
    *logOutStream << s1 << Qt::endl;
    qDebug() << s1;
}


void MainWindow::ReadinParFile(QString fName)
{
    QString fullName = currentPath + fName;
    QFile file(fullName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        LogOut("unopen parameter file!");
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();

        if(line.isEmpty() || line.startsWith("#"))
        {
            continue;
        }

        if(line.contains("#"))
        {
            QStringList ss = line.split("#");
            line = ss[0];
        }

        // 去除首尾空格，并按等号进行分割
        QStringList parts = line.simplified().split("=");

        if (2 == parts.size())
        {
            QString key = parts[0].trimmed(); // 去除键的首尾空格
            QString value = parts[1].trimmed(); // 去除值的首尾空格
            pars[key] = value;
            //qDebug() << "Key:" << key << ", Value:" << value;
        }
        else
        {
            qDebug() << "Invalid format!";
        }
    }
}


void MainWindow::UpdateProgressBar(int pos)
{
    ui->progressBar_readinData->setValue(pos);
}


void MainWindow::ReStoreData()
{
    m_xList.clear();
    m_yList.clear();
    m_eList.clear();

    auto data = dataObject->GetData();

    int nEvts = data[0].size();
    for (int i = 0; i < nEvts; ++i)
    {
        quint16 x = data[0][i];
        quint16 y = data[1][i];
        quint16 e = data[2][i];

        quint16 xx = std::round(factor*x/e*nPixel) - bias;
        quint16 yy = std::round(factor*y/e*nPixel) - bias;

        if(0<xx && xx<nPixel && 0<yy && yy<nPixel && 0<e && e<maxADC)
        {
            m_xList.append(xx);
            m_yList.append(yy);
            m_eList.append(e);
        }

    }

    // 保存数据
    QString fName = currentPath + "Data/data_CM0_BK0.bin";
    QFile ofile(fName);
    if (ofile.open(QIODevice::WriteOnly))
    {
        QDataStream out(&ofile);
        for (int i = 0; i < m_eList.size(); ++i)
        {
            out << m_xList[i] << m_yList[i] << m_eList[i];
        }
        ofile.close();
    }
    else
    {
        LogOut("Readout file fail to create.");
        return;
    }

    LogOut("按 BK 保存文件完成。");


    /******************************************************
    //保存数据
    for (int iFile = 0; iFile<data.size(); ++iFile)
    {
        int CMID = iFile/nBK;
        int BKID = iFile%nBK;

        QString outFileName =
            QString("./Data/data_CM%1_BK%2.bin").arg(CMID).arg(BKID);
        QFile outFile(outFileName);

        if (outFile.open(QIODevice::WriteOnly))
        {
            // 创建数据流
            QDataStream out(&outFile);
            foreach (auto var, data[iFile])
            {
                out << var;
            }
            // out << data[iFile];
            outFile.close();
        }
    }
    ***************************************************/
}

