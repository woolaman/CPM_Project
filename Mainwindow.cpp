#include "Mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QtCore>
#include <QFile>
#include <QPixmap>
#include <QPainter>
#include <QDateTime>
#include <QDir>
#include <QMessageBox>
#include <QThread>

#include <opencv2/imgproc.hpp>

#include "Parameters.h"


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

    m_BK = new Block();

    eHistLine = new QLineSeries();
    eHistLine->setPen( QPen(Qt::black, 1) );

    for (int i = 0; i < ADC_nBins; ++i)
    {
        qreal x = ADC_min + i*ADC_binWidth + ADC_binWidth/2;
        eHistLine->append(x, 0);
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

    axisX->setRange(0, ADC_max);
    axisX->setTickCount(ADC_max/10000+1);
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

    ShowImage(cv::Mat::zeros(nPixel, nPixel, CV_16UC1));
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

    //delete dataObject;

    LogOut("程序关闭。");

    delete logOutStream;
    logFile->close();
}


// 一些功能性函数
cv::Mat MainWindow::GetColorMap(cv::Mat_<qreal> I)
{
    cv::Mat_<quint8> scaledMap(I.size(), 0);

    qreal max_val;
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


void MainWindow::ShowImage(cv::Mat_<qreal> I)
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


void MainWindow::ShowPeaks(cv::Mat_<qreal> I, cv::Mat_<cv::Vec2w> pt)
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


void MainWindow::on_pushButton_readinData_clicked()
{
    LogOut("开始读入数据...");
    QString fName = ui->lineEdit_dataPath->text();
    fName.replace("\\", "/");
    qDebug() << "fName = " << fName;

    ui->progressBar_readinData->setMinimum(0);
    ui->progressBar_readinData->setMaximum(100);
    ui->progressBar_readinData->setValue(0);

    dataObject = new Readin(fName);

    QThread* thread = new QThread(); // 创建新线程
    dataObject->moveToThread(thread);

    // 连接 Worker 对象的工作完成信号到线程的退出槽
    // QObject::connect(&worker, &Worker::workFinished, &thread, &QThread::quit);

    // 连接线程的启动信号到 Worker 对象的工作槽
    QObject::connect(thread, &QThread::started, dataObject, &Readin::StartReadTxt);

    QObject::connect(dataObject, SIGNAL(currentPos(int)),  this, SLOT(UpdateProgressBar(int)));
    QObject::connect(dataObject, SIGNAL(finished()), this, SLOT(ReStoreData()));

    // 当线程结束时，删除 Worker 对象和线程对象
    //QObject::connect(&thread, &QThread::finished, &worker, &QObject::deleteLater);
    //QObject::connect(&thread, &QThread::finished, &thread, &QObject::deleteLater);

    // 启动新线程
    thread->start();
}


void MainWindow::on_pushButton_setEW_clicked()
{
    QString fName = currentPath + "Data/data.bin";
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

        QDataStream in(&file);
        while (!in.atEnd())
        {
            in >> x >> y >> e;
            m_BK->Fill(x, y, e);
        }
        file.close();
    }
    else
    {
        LogOut("文件打开失败，请检查文件是否正确。");
        return;
    }

    Histogram ADCHist = m_BK->GetADCHist();

    QVector<QPointF> points;
    for (int i = 0; i < ADC_nBins; ++i)
    {
        qreal x = ADCHist.GetBinCenter(i);
        qreal y = ADCHist.GetBinContent(i);
        points.append(QPointF(x, y));
    }

    QPointF peak = ADCHist.GetPeak();
    qreal peak_x = peak.x();
    m_peakValue = peak.y();

    eHistLine->replace(points);

    int minEW = qRound(peak_x*(1-EW_width));
    int maxEW = qRound(peak_x*(1+EW_width));

    axisX->setRange(0, ADC_max);
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
    m_BK->SetEW(minEW, maxEW);
    m_BK->CalMap();
    ShowImage(m_BK->GetMap());
    LogOut("Generate I0. ");

    peakE = ui->lineEdit_peakEnergy->text().toInt();

    //cv::Mat color_I0 = GetColorMap(I0);
    //cv::namedWindow("I0", cv::WINDOW_NORMAL); // cv::WINDOW_AUTOSIZE
    //cv::imshow("I0", color_I0);
}


void MainWindow::on_pushButton_segment_clicked()
{
    LogOut("开始分割...");
    imgFlag = 0;
    m_BK->Segment("FindMaximum");
    ShowPeaks(m_BK->GetMap(), m_BK->GetPeakTable());
}


// 此函数生成整机可用的位置查找表，即将各个BK的分割结果汇总起来变成一个文件
void MainWindow::on_pushButton_genPositionLUT_clicked()
{
    m_BK->GenPositionLUT();
}


void MainWindow::on_pushButton_genEnergyLUT_clicked()
{

    m_BK->CalRecEHist();
    m_BK->GenEnergyLUT();
}


void MainWindow::on_pushButton_calPeaks_clicked()
{
    // 调取能谱数据，查看单根晶体刻度过程中，自动寻峰结果是否完全正确
    // 如果不正确可以修改，并重新生成能量查找表
    imgFlag = 1;

    EWLeftLine->replace(0, -1, 0);
    EWLeftLine->replace(1, -1, 1);

    EWRightLine->replace(0, -1, 0);
    EWRightLine->replace(1, -1, 1);

    ShowEHist();
}


void MainWindow::ShowEHist(int peakLoc)
{
    int m = ui->lineEdit_colID->text().toInt();
    int n = ui->lineEdit_rowID->text().toInt();
    int crystalID = n*nCrystal + m;

    Crystal* aCrystal = m_BK->GetCrystal(crystalID);
    Histogram ADCHist = aCrystal->GetADCHist();

    ADCHist.Smooth(10, 2);

    QVector<QPointF> points;
    for (int i = 0; i < ADC_nBins; ++i)
    {
        qreal x = ADCHist.GetBinCenter(i);
        qreal y = ADCHist.GetBinContent(i);
        points.append(QPointF(x, y));
    }
    eHistLine->replace(points);

    if(-1==peakLoc)
    {
        peakLine->replace(0, ADCHist.GetPeak().x(), 0);
        peakLine->replace(1, ADCHist.GetPeak().x(), ADCHist.GetPeak().y());
    }
    else
    {
        peakLine->replace(0, peakLoc, 0);
        peakLine->replace(1, peakLoc, ADCHist.GetPeak().y());
    }

    axisY->setRange(0, ADCHist.GetPeak().y()*1.1);

    QPixmap pixmap = chartView->grab();
    ui->label_floodmap->setPixmap(pixmap);
}


void MainWindow::on_pushButton_calOnePeak_clicked()
{
    // 只读取能谱文件中的某一段数据，即某个晶体的能谱数据
    imgFlag = 2;

    EWLeftLine->replace(0, -1, 0);
    EWLeftLine->replace(1, -1, 1);

    EWRightLine->replace(0, -1, 0);
    EWRightLine->replace(1, -1, 1);

    //按照界面中设置的行号和列号，读入该晶体的能谱
    int rowID = ui->lineEdit_rowID->text().toInt();
    int colID = ui->lineEdit_colID->text().toInt();
    int crystalID = rowID*nCrystal + colID;

    Crystal* aCrystal = m_BK->GetCrystal(crystalID);
    Histogram ADCHist = aCrystal->GetADCHist();
    ADCHist.Smooth();

    QVector<QPointF> points;
    for (int i = 0; i < ADC_nBins; ++i)
    {
        qreal x = ADCHist.GetBinCenter(i);
        qreal y = ADCHist.GetBinContent(i);
        points.append(QPointF(x, y));
    }
    eHistLine->replace(points);

    peakLine->replace(0, ADCHist.GetPeak().x(), 0);
    peakLine->replace(1, ADCHist.GetPeak().x(), ADCHist.GetPeak().y());

    axisY->setRange(0, ADCHist.GetPeak().y()*1.1);

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

        m_BK->ManualAdjust(pos);
        ShowPeaks(m_BK->GetMap(), m_BK->GetPeakTable());
    }

    if(1==imgFlag || 2==imgFlag)
    {
        QPoint pos = ui->label_floodmap->GetPos();
        auto coordinate = chart->mapToValue(pos);
        quint16 peakLoc = quint16(coordinate.x());
        ShowEHist(peakLoc);

        int rowID = ui->lineEdit_rowID->text().toInt();
        int colID = ui->lineEdit_colID->text().toInt();
        int crystalID = rowID*nCrystal + colID;

        qreal slope = peakE/peakLoc;
        m_BK->GetCrystal(crystalID)->SetSlope(slope);
    }
}


void MainWindow::on_label_floodmap_mouseRightClicked()
{
    if(0==imgFlag)
    {
        m_BK->GenSegResult();
        ShowImage(m_BK->GetSegMap());
        // 保存分割结果图片
        QPixmap pixmap = ui->label_floodmap->pixmap();
        QImage image = pixmap.toImage();
        QString figName = "./Data/segResult.png";
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
    m_BK->CalSegFOM();
    qreal IR = m_BK->GetIR();
    qreal meanRMS = m_BK->GetRMS();
    ui->lineEdit_IR->setText(QString::number(IR, 'f', 2));
    ui->lineEdit_RMS->setText(QString::number(meanRMS, 'f', 2));
}


void MainWindow::on_pushButton_writePeaks_clicked()
{
    /**********************************
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
    *******************************/
}


void MainWindow::on_pushButton_calEnergyResolution_clicked()
{
    // 计算单根分辨率
    QVector<qreal> ERs;
    for (int i = 0; i < crystalNum; ++i)
    {
        Crystal* iCrystal = m_BK->GetCrystal(i);
        qreal aER =  iCrystal->GetER();
        ERs.append(aER);
    }

    // 画单根分辨率map 在label_floodmap
    cv::Mat_<qreal> ERMat(nCrystal, nCrystal);
    for (int iRow = 0; iRow < nCrystal; ++iRow)
    {
        for (int iCol = 0; iCol < nCrystal; ++iCol)
        {
            qreal aER = ERs[iRow*nCrystal + iCol];
            ERMat(iRow, iCol) = aER;
        }
    }

    ShowImage(ERMat);

    // 计算总分辨率
    //m_BK->CalRecEHist();
    qreal totalER = m_BK->GetER()*100;
    Histogram eHist = m_BK->GetRecEHist();

    // 画总能谱在 label_ehist
    QVector<QPointF> points;
    for (int i = 0; i < recE_nBins; ++i)
    {
        qreal x = eHist.GetBinCenter(i);
        qreal y = eHist.GetBinContent(i);
        points.append( QPointF(x, y) );
    }
    eHistLine->replace(points);

    qreal leftValue = eHist.GetLeftValue();
    qreal rightValue = eHist.GetRightValue();
    qreal peakValue = eHist.GetPeak().y();

    EWLeftLine->replace(0, leftValue, 0);
    EWLeftLine->replace(1, leftValue, peakValue);

    EWRightLine->replace(0, rightValue, 0);
    EWRightLine->replace(1, rightValue, peakValue);

    axisX->setRange(recE_min, recE_max);
    axisY->setRange(0, qRound(peakValue*1.1));

    chartView->resize(ui->label_eHist->size());

    QPixmap pixmap = chartView->grab();
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::red, 1));

    painter.setFont(QFont("Arial", 12));
    painter.drawText(QPoint(300, 100), QString::number(totalER, 'f', 2) + " %");

    ui->label_eHist->setPixmap(pixmap);
    LogOut("total energy resolution is " + QString::number(totalER, 'f', 2) + " %.");
}


void MainWindow::on_pushButton_calUniformity_clicked()
{
    cv::Mat_<qreal> nEvts = cv::Mat::zeros(nCrystal, nCrystal, CV_32FC1);

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

    qreal mean_nEvts = 1.0*totalEvts/(crystalNum);
    cv::Mat_<qreal> uniformityPar = mean_nEvts/nEvts;

    QFile outfile(fName_LUT_U);
    if ( outfile.open(QIODevice::WriteOnly) )
    {
        QDataStream out(&outfile);

        for (int iRow = 0; iRow < nCrystal; ++iRow)
        {
            for (int iCol = 0; iCol < nCrystal; ++iCol)
            {
                qreal c = uniformityPar(iRow, iCol);
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


void MainWindow::UpdateProgressBar(int pos)
{
    ui->progressBar_readinData->setValue(pos);
}


void MainWindow::ReStoreData()
{
    auto data = dataObject->GetData();

    int nEvts = data[0].size();
    for (int i = 0; i < nEvts; ++i)
    {
        quint16 x = data[0][i];
        quint16 y = data[1][i];
        quint16 e = data[2][i];

        quint16 xx = std::round(factor*x/e*nPixel) - bias;
        quint16 yy = std::round(factor*y/e*nPixel) - bias;

        //if(0<xx && xx<nPixel && 0<yy && yy<nPixel && 0<e && e<ADC_max)
        if(xmin<xx && xx<xmax && ymin<yy && yy<ymax && 0<e && e<ADC_max)
        {
            m_BK->Fill(xx, yy, e);
        }
    }

    QString fName = currentPath + "Data/data.bin";
    m_BK->SaveToFile(fName);
}

