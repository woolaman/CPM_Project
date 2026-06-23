#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <QtCore/QtCore>
#include <QFile>
#include <QPixmap>
#include <QPainter>
#include <QDateTime>
#include <QDir>
#include <QMessageBox>
#include <QThread>
#include <QGuiApplication>  // 用于 QGuiApplication::primaryScreen()
#include <QWindow>          // 用于 QWindow 指针
#include <QScreen>
#include <opencv2/imgproc.hpp>
#include "Parameters.h"
#include <Windows.h>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	qDebug() << QString::fromLocal8Bit("初始化开始...");

	ui->setupUi(this);

	m_BK = new Block();

	eHistLine = new QtCharts::QLineSeries();
	eHistLine->setPen(QPen(Qt::black, 1));

	for (int i = 0; i < ADC_nBins; ++i)
	{
		qreal x = ADC_min + i * ADC_binWidth + ADC_binWidth / 2;
		eHistLine->append(x, -1);
	}

	EWLeftLine = new QtCharts::QLineSeries();
	EWLeftLine->setPen(QPen(Qt::red, 1));
	EWLeftLine->append(-1, 0);
	EWLeftLine->append(-1, 1);

	EWRightLine = new QtCharts::QLineSeries();
	EWRightLine->setPen(QPen(Qt::red, 1));
	EWRightLine->append(-1, 0);
	EWRightLine->append(-1, 1);

	peakLine = new QtCharts::QLineSeries();
	peakLine->setPen(QPen(Qt::red, 1));
	peakLine->append(-1, 0);
	peakLine->append(-1, 1);

	chart = new QtCharts::QChart();
	chartView = new QtCharts::QChartView();

	axisX = new QtCharts::QValueAxis();
	axisY = new QtCharts::QValueAxis();

	chart->addSeries(eHistLine);
	chart->addSeries(EWLeftLine);
	chart->addSeries(EWRightLine);
	chart->addSeries(peakLine);

	chart->setMargins(QMargins(0, 0, 0, 0));
	chart->legend()->setVisible(false);

	axisX->setRange(0, ADC_max);
	//axisX->setTickCount(11);
	axisX->applyNiceNumbers();
	axisX->setLabelFormat("%d");

	axisY->setRange(0, 100);
	//axisY->setTickCount(6);
	axisY->applyNiceNumbers();
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

	ShowImage(cv::Mat::zeros(nPixel, nPixel, CV_64FC1));
	qDebug() << QString::fromLocal8Bit("初始化结束。");

	floodmapWidget = new window_floodmap();
	QObject::connect(floodmapWidget, &window_floodmap::closeClicked, this, &MainWindow::on_FloodmapClosed);
}

MainWindow::~MainWindow()
{
	qDebug() << QString::fromLocal8Bit("程序关闭。");
}

// 一些功能性函数
cv::Mat MainWindow::GetColorMap(cv::Mat_<qreal> I)
{
	float c = 2.5;
	cv::Mat_<quint8> scaledMap(I.size(), 0);

	qreal minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(I, &minVal, &maxVal, &minLoc, &maxLoc);

	for (int i = 0; i < I.rows; ++i)
	{
		for (int j = 0; j < I.cols; ++j)
		{
			int value = qRound(1.0 * I(i, j) / maxVal * 255 * c);
			if (value > 255)
			{
				value = 255;
			}
			scaledMap(i, j) = static_cast<quint8>(value);
		}
	}

	cv::Mat colorMap;
	cv::applyColorMap(scaledMap, colorMap, cv::COLORMAP_HOT);

	return colorMap;
}

void MainWindow::ShowImage(cv::Mat_<qreal> I)
{
	cv::Mat colorMap = GetColorMap(I);
	//QImage qtImage(colorMap.data, colorMap.cols, colorMap.rows,
	//               colorMap.step, QImage::Format_BGR888);
	QImage qtImage(colorMap.data, colorMap.cols, colorMap.rows,
		colorMap.step, QImage::Format_RGB888);
	qtImage = qtImage.rgbSwapped();

	if (qtImage.width() < nPixel || qtImage.height() < nPixel)
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
			int x = pt(j, i)[0];
			int y = pt(j, i)[1];

			int r = 4;
			painter.drawLine(x - r, y, x + r, y); // "——"
			//painter.drawLine(x, y-r, x, y+r); // "|"
			points.append(QPoint(x, y));
		}
		painter.drawPolyline(points.data(), points.size());
	}

	ui->label_floodmap->setPixmap(pixmap);
}

void MainWindow::on_pushButton_readinData_clicked()
{
	qDebug() << "Readin data start ...";
	QString fName = ui->lineEdit_dataPath->text();
	fName.replace("\\", "/");
	fName = fName.trimmed().remove(QChar('\"'));
	qDebug() << "fName = " << fName;

	ui->progressBar_readinData->setMinimum(0);
	ui->progressBar_readinData->setMaximum(100);
	ui->progressBar_readinData->setValue(0);

	dataObject = new Readin(fName);

	QThread* thread = new QThread(); // 创建新线程
	dataObject->moveToThread(thread);

	// 连接线程的启动信号到对象的工作槽
	QObject::connect(thread, &QThread::started, dataObject, &Readin::StartReadData);
	QObject::connect(dataObject, SIGNAL(currentPos(int)), this, SLOT(UpdateProgressBar(int)));
	QObject::connect(dataObject, SIGNAL(ShowMessage(QString, QString)), this, SLOT(ShowMessage(QString, QString)));

	// 当线程结束时，删除对象和线程对象
	QObject::connect(thread, &QThread::finished, dataObject, &QObject::deleteLater);
	QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);

	// 启动新线程
	thread->start();
}

void MainWindow::on_pushButton_setEW_clicked()
{
	QString strCMID = ui->lineEdit_CMID->text().rightJustified(2, '0');
	QString strBKID = ui->lineEdit_BKID->text().rightJustified(2, '0');

	int CMID = strCMID.toInt();
	int BKID = strBKID.toInt();

	m_BK->Clear();
	m_BK->SetID(CMID * nBK + BKID);

	QString fName = currentPath + "Data/s_" + strCMID + "_" + strBKID;
	if (!QFile::exists(fName))
	{
		qDebug() << "file does not exist, please chech file!";
		return;
	}

	qDebug() << QString::fromLocal8Bit("开始读入数据...") << fName;
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
			if (0 < x && x < 512 && 0 < y && y < 512 && ADC_min < e && e < ADC_max)
			{
				m_BK->Fill(x, y, e);
			}
		}
		file.close();
		qDebug() << QString::fromLocal8Bit("读入数据完毕。");
	}
	else
	{
		qDebug() << QString::fromLocal8Bit("打开文件失败! ");
		return;
	}

	qDebug() << "Draw total ADC histogram in label";
	Histogram* ADCHist = m_BK->GetADCHist()->Clone();
	ADCHist->Smooth();

	QVector<QPointF> points;
	for (int i = 0; i < ADC_nBins; ++i)
	{
		qreal x = ADCHist->GetBinCenter(i);
		qreal y = ADCHist->GetBinContent(i);
		points.append(QPointF(x, y));
	}

	qDebug() << "find peak, then draw peak line.";
	ADCHist->SetCutValue(ADC_cutValue);
	QPointF peak = ADCHist->GetPeak();
	m_peakLoc = peak.x();
	m_peakValue = peak.y();

	eHistLine->replace(points);

	qDebug() << "setup energy window.";
	int minEW = qRound(m_peakLoc * (1 - EW_width));
	int maxEW = qRound(m_peakLoc * (1 + EW_width));

	//axisX->setRange(0, ADC_max);
	axisY->setRange(0, qRound(m_peakValue * 1.1));

	EWLeftLine->replace(0, minEW, 0);
	EWLeftLine->replace(1, minEW, m_peakValue);

	EWRightLine->replace(0, maxEW, 0);
	EWRightLine->replace(1, maxEW, m_peakValue);

	qDebug() << "set pixel map.";
	QPixmap pixmap = chartView->grab();
	ui->label_eHist->setPixmap(pixmap);

	ui->lineEdit_minEW->setText(QString::number(minEW));
	ui->lineEdit_maxEW->setText(QString::number(maxEW));
	qDebug() << "draw bk ADC histogram, and auto-gen 25% energy window.";

	// 参照能窗的设置参数，筛选数据
	m_BK->CalMap(minEW, maxEW);
	ShowImage(m_BK->GetMap());
	ui->label_floodmap->setMouseTracking(true);
	qDebug() << "Generate I0. ";

	// save floodmap to file
	QPixmap floodmap_pixmap = *(ui->label_floodmap->pixmap());
	QImage floodmap_image = floodmap_pixmap.toImage();
	QString figName = "./Data/Floodmap_" + strCMID + "_" + strBKID + ".png";
	if (floodmap_image.save(figName))
	{
		qDebug() << "Image saved successfully to" << figName;
	}
	else
	{
		qDebug() << "Failed to save image to" << figName;
	}
}

void MainWindow::on_pushButton_segment_clicked()
{
	qDebug() << QString::fromLocal8Bit("开始分割...");
	imgFlag = 0;
	m_BK->Segment(segMethod);
	ShowPeaks(m_BK->GetMap(), m_BK->GetPeakTable());
	ui->label_floodmap->setMouseTracking(false);

	if (floodmapWidget == nullptr)
	{
		floodmapWidget = new window_floodmap();
	}
	floodmapWidget->show();
	floodmapWidget->SetFloodmap(m_BK->GetMap());
	floodmapWidget->SetPeakTable(m_BK->GetPeakTable());
	floodmapWidget->ShowPeaks();
}

// 此函数生成整机可用的位置查找表，即将各个BK的分割结果汇总起来变成一个文件
void MainWindow::on_pushButton_genPositionLUT_clicked()
{
	for (size_t iDPB = 0; iDPB < 10; iDPB++)
	{
		QString ofName = QString("./") + QStringLiteral("查找表") + "/PLUT" + QString::number(iDPB);
		QFile ofile(ofName);
		if (!ofile.open(QIODevice::WriteOnly))
		{
			qDebug() << "打开文件失败，无法生成位置查找表。";
			return;
		}

		for (size_t iDM = 0; iDM < 2; iDM++)
		{
			int DMID = iDPB * 2 + iDM;
			QString DMID_str = QString::number(DMID).rightJustified(2, '0');

			for (size_t iBK = 0; iBK < nBK; iBK++)
			{
				QString BKID_str = QString::number(iBK).rightJustified(2, '0');

				QString inFileName = currentPath + "Data/PLUT_" + DMID_str + "_" + BKID_str;
				QFile inFile(inFileName);
				if (!inFile.open(QIODevice::ReadOnly))
				{
					qDebug() << "打开文件失败，无法生成位置查找表。";
					return;
				}
				QByteArray fileData = inFile.readAll();
				ofile.write(fileData);
				inFile.close();
			}
		}
		ofile.close();
	}

	QMessageBox::information(this, "Position LUT", "Position LUT is saved to disk.");
}

void MainWindow::on_pushButton_genEnergyLUT_clicked()
{
	for (size_t iDPB = 0; iDPB < 10; iDPB++)
	{
		QString ofName = QString("./") + QStringLiteral("查找表") + "/B3PET_CM" + QString::number(iDPB) + "_EW";
		QFile ofile(ofName);
		if (!ofile.open(QIODevice::WriteOnly))
		{
			qDebug() << "打开文件失败，无法生成能量查找表。";
			return;
		}

		for (size_t iDM = 0; iDM < 2; iDM++)
		{
			int DMID = iDPB * 2 + iDM;
			QString DMID_str = QString::number(DMID).rightJustified(2, '0');

			for (size_t iBK = 0; iBK < nBK; iBK++)
			{
				QString BKID_str = QString::number(iBK).rightJustified(2, '0');

				QString inFileName = currentPath + "Data/ELUT_" + DMID_str + "_" + BKID_str;
				QFile inFile(inFileName);
				if (!inFile.open(QIODevice::ReadOnly))
				{
					qDebug() << "打开文件失败，无法生成能量查找表。";
					return;
				}
				QByteArray fileData = inFile.readAll();
				ofile.write(fileData);
				inFile.close();
			}
		}
		ofile.close();
	}

	QMessageBox::information(this, "Energy LUT", "Energy LUT is saved to disk.");
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

	//load e histogram from file
	QString strCMID = ui->lineEdit_CMID->text().rightJustified(2, '0');
	QString strBKID = ui->lineEdit_BKID->text().rightJustified(2, '0');
	int CMID = strCMID.toInt();
	int BKID = strBKID.toInt();

	m_BK->Clear();
	m_BK->SetID(CMID * nBK + BKID);

	QString fName = currentPath + "Data/EHist_" + strCMID + "_" + strBKID;
	QFile file(fName);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << QString::fromLocal8Bit("能谱文件读入失败！");
		return;
	}

	QDataStream inStream(&file);
	inStream.setByteOrder(QDataStream::LittleEndian); // 如果需要小端序

	for (int iCrystal = 0; iCrystal < crystalNum; ++iCrystal)
	{
		auto aCrystal = m_BK->GetCrystal(iCrystal);

		QVector<qreal> aHist(ADC_nBins, 0.0);
		for (size_t iBin = 0; iBin < ADC_nBins; iBin++)
		{
			quint16 temp;
			inStream >> temp;
			aHist[iBin] = 1.0 * temp;
		}

		aCrystal->GetADCHist()->SetBinContents(aHist);
		aCrystal->CalSlope();
	}

	file.close();

	ShowADCHist();
}

void MainWindow::ShowADCHist(int peakLoc)
{
	int m = ui->lineEdit_colID->text().toInt();
	int n = ui->lineEdit_rowID->text().toInt();
	int crystalID = n * nCrystal + m;
	Histogram* ADCHist = m_BK->GetCrystal(crystalID)->GetADCHist()->Clone();
	ADCHist->Smooth(10, 2);
	ADCHist->SetCutValue(ADC_cutValue);

	QVector<QPointF> points;
	for (int i = 0; i < ADC_nBins; ++i)
	{
		qreal x = ADCHist->GetBinCenter(i);
		qreal y = ADCHist->GetBinContent(i);
		points.append(QPointF(x, y));
	}
	eHistLine->replace(points);

	if (-1 == peakLoc)
	{
		peakLine->replace(0, ADCHist->GetPeak().x(), 0);
		peakLine->replace(1, ADCHist->GetPeak().x(), ADCHist->GetPeak().y());
	}
	else
	{
		peakLine->replace(0, peakLoc, 0);
		peakLine->replace(1, peakLoc, ADCHist->GetPeak().y());
	}

	axisX->setRange(ADC_min, ADC_max);
	axisX->applyNiceNumbers();
	axisY->setRange(0, ADCHist->GetPeak().y() * 1.1);
	axisY->applyNiceNumbers();

	QPixmap pixmap = chartView->grab();
	ui->label_floodmap->setPixmap(pixmap);
}

void MainWindow::on_pushButton_calOnePeak_clicked()
{
	imgFlag = 2;

	EWLeftLine->replace(0, -1, 0);
	EWLeftLine->replace(1, -1, 1);

	EWRightLine->replace(0, -1, 0);
	EWRightLine->replace(1, -1, 1);

	//按照界面中设置的行号和列号，读入该晶体的能谱
	int rowID = ui->lineEdit_rowID->text().toInt();
	int colID = ui->lineEdit_colID->text().toInt();
	int crystalID = rowID * nCrystal + colID;

	Crystal* aCrystal = m_BK->GetCrystal(crystalID);
	Histogram* ADCHist = aCrystal->GetADCHist()->Clone();
	ADCHist->Smooth(10, 2);

	QVector<QPointF> points;
	for (int i = 0; i < ADC_nBins; ++i)
	{
		qreal x = ADCHist->GetBinCenter(i);
		qreal y = ADCHist->GetBinContent(i);
		points.append(QPointF(x, y));
	}
	eHistLine->replace(points);

	peakLine->replace(0, ADCHist->GetPeak().x(), 0);
	peakLine->replace(1, ADCHist->GetPeak().x(), ADCHist->GetPeak().y());

	axisX->setRange(ADC_min, ADC_max);
	axisX->applyNiceNumbers();
	axisY->setRange(0, ADCHist->GetPeak().y() * 1.1);

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
	if (imgFlag == 0)
	{
		// step 3, 手动修正 peak 位置
		QPoint pos = ui->label_floodmap->GetPos();
		m_BK->ManualAdjust(pos);
		ShowPeaks(m_BK->GetMap(), m_BK->GetPeakTable());
	}

	// 手动修正能谱  
	if (imgFlag == 1 || imgFlag == 2)
	{
		QPoint pos = ui->label_floodmap->GetPos();
		auto coordinate = chart->mapToValue(pos);
		quint16 peakLoc = quint16(coordinate.x());
		ShowADCHist(peakLoc);

		int rowID = ui->lineEdit_rowID->text().toInt();
		int colID = ui->lineEdit_colID->text().toInt();
		int crystalID = rowID * nCrystal + colID;

		qreal slope = peakE / peakLoc;
		m_BK->GetCrystal(crystalID)->SetSlope(slope);
	}

	if (imgFlag == 3)
	{
		// nothing to do
	}
}

void MainWindow::on_label_floodmap_mouseRightClicked()
{
	if (imgFlag == 0)
	{
		m_BK->CalSegResult();
		ShowImage(m_BK->GetSegMap());
		// 保存分割结果图片
		QPixmap pixmap = *(ui->label_floodmap->pixmap());
		QImage image = pixmap.toImage();

		QString strCMID = ui->lineEdit_CMID->text().rightJustified(2, '0');
		QString strBKID = ui->lineEdit_BKID->text().rightJustified(2, '0');

		QString figName = "./Data/SegResult_" + strCMID + "_" + strBKID + ".png";
		if (image.save(figName))
		{
			qDebug() << "Image saved successfully to" << figName;
		}
		else
		{
			qDebug() << "Failed to save image to" << figName;
		}

		QMessageBox::information(this, "Segment", "Segment is done.");
	}

	if (imgFlag == 1)
	{
		int m = ui->lineEdit_colID->text().toInt();
		int n = ui->lineEdit_rowID->text().toInt();

		if (n >= nCrystal || m >= nCrystal)
		{
			qDebug() << "Row or col ID is out of range.";
			return;
		}

		if (m == nCrystal - 1)
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

		ShowADCHist();
	}

	if (imgFlag == 2)
	{
		// nothing to do!
	}

	if (imgFlag == 3)
	{
		int m = ui->lineEdit_colID->text().toInt();
		int n = ui->lineEdit_rowID->text().toInt();

		if (n >= nCrystal || m >= nCrystal)
		{
			qDebug() << "Row or col ID is out of range.";
			return;
		}

		if (m == nCrystal - 1)
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

		ShowRecEHist();
	}
}

void MainWindow::on_pushButton_calSegFOM_clicked()
{
	m_BK->CalSegFOM();
	qreal PVR = m_BK->GetPVR();
	qreal FWHM = m_BK->GetFWHM();
	ui->lineEdit_PVR->setText(QString::number(PVR, 'f', 2));
	ui->lineEdit_FWHM->setText(QString::number(FWHM, 'f', 2));
}

void MainWindow::on_pushButton_writePeaks_clicked()
{
	m_BK->ReGenELUT();
}

void MainWindow::on_pushButton_calEnergyResolution_clicked()
{
	// 计算单根分辨率
	// 画单根分辨率map 在label_floodmap
	qDebug() << "Calculate single crystal's energy resolution.";
	cv::Mat_<qreal> ERMat(nCrystal, nCrystal);
	cv::Mat_<qreal> peakLocMat(nCrystal, nCrystal);

	for (int iRow = 0; iRow < nCrystal; ++iRow)
	{
		for (int iCol = 0; iCol < nCrystal; ++iCol)
		{
			int id = iRow * nCrystal + iCol;
			qreal aER = m_BK->GetCrystal(id)->GetER();
			ERMat(iRow, iCol) = aER * 100;

			qreal slope = m_BK->GetCrystal(id)->GetSlope();
			peakLocMat(iRow, iCol) = peakE / slope;
		}
	}

	// show peak location map
	//cv::Scalar mean, stdDev;
	//cv::meanStdDev(peakLocMat, mean, stdDev);
	//qreal meanVal = mean.val[0];
	//qreal stdDevVal = stdDev.val[0];
	//qDebug() << "mean peak location = " << meanVal;

	qDebug() << "total ADC histogram peak location = " << m_peakLoc;
	cv::Mat_<qreal> relativeLoc = peakLocMat / m_peakLoc;
	ShowImage(relativeLoc);

	QPixmap pixmap0 = *(ui->label_floodmap->pixmap());
	QPainter painter0(&pixmap0);
	painter0.setPen(QPen(Qt::black, 1));

	int fontSize = 10;
	if (nCrystal < 10)
	{
		fontSize = 12;
	}
	else if (nCrystal < 20)
	{
		fontSize = 10;
	}
	else
	{
		fontSize = 8;
	}

	painter0.setFont(QFont("Arial", 6));

	qreal width = 1.0 * nPixel / nCrystal;
	for (int iRow = 0; iRow < nCrystal; ++iRow)
	{
		for (int iCol = 0; iCol < nCrystal; ++iCol)
		{
			QRect rect(iCol * width, iRow * width, width, width);
			painter0.drawText(rect, Qt::AlignCenter,
				QString::number(relativeLoc(iRow, iCol), 'f', 2));
		}
	}

	QString strCMID = ui->lineEdit_CMID->text().rightJustified(2, '0');
	QString strBKID = ui->lineEdit_BKID->text().rightJustified(2, '0');
	QString figName;

	// 保存图片
	QImage image0 = pixmap0.toImage();
	figName = "./Data/PeakLoc_" + strCMID + "_" + strBKID + ".png";
	image0.save(figName);

	// show energy resolution map
	ShowImage(ERMat);
	QPixmap pixmap1 = *(ui->label_floodmap->pixmap());
	QPainter aPainter(&pixmap1);
	aPainter.setPen(QPen(Qt::black, 1));
	aPainter.setFont(QFont("Arial", fontSize));

	for (int iRow = 0; iRow < nCrystal; ++iRow)
	{
		for (int iCol = 0; iCol < nCrystal; ++iCol)
		{
			int iER = qRound(ERMat(iRow, iCol));
			QRect rect(iCol * width, iRow * width, width, width);
			aPainter.drawText(rect, Qt::AlignCenter, QString::number(iER));
		}
	}
	ui->label_floodmap->setPixmap(pixmap1);

	// 保存图片
	QImage image1 = pixmap1.toImage();
	figName = "./Data/ER_Mat_" + strCMID + "_" + strBKID + ".png";
	image1.save(figName);


	// 计算总分辨率
	qDebug() << "Calculate the whole block energy resolution.";
	qreal totalER = m_BK->GetER() * 100;
	qDebug() << "total ER = " << totalER << " %";
	Histogram* eHist = m_BK->GetRecEHist();

	// 画总能谱在 label_ehist
	QVector<QPointF> points;
	for (int i = 0; i < recE_nBins; ++i)
	{
		qreal x = eHist->GetBinCenter(i);
		qreal y = eHist->GetBinContent(i);
		points.append(QPointF(x, y));
	}
	eHistLine->replace(points);

	qreal leftValue = eHist->GetLeftValue();
	qreal rightValue = eHist->GetRightValue();
	qreal peakValue = eHist->GetPeak().y();

	EWLeftLine->replace(0, leftValue, 0);
	EWLeftLine->replace(1, leftValue, peakValue);

	EWRightLine->replace(0, rightValue, 0);
	EWRightLine->replace(1, rightValue, peakValue);

	axisX->setRange(recE_min, recE_max);
	axisY->setRange(0, qRound(peakValue * 1.1));

	chartView->resize(ui->label_eHist->size());

	QPixmap pixmap2 = chartView->grab();
	QPainter painter2(&pixmap2);
	painter2.setPen(QPen(Qt::red, 1));

	painter2.setFont(QFont("Arial", 12));
	painter2.drawText(QPoint(300, 100), QString::number(totalER, 'f', 2) + " %");

	ui->label_eHist->setPixmap(pixmap2);
	qDebug() << "total energy resolution is " <<
		QString::number(totalER, 'f', 2) + " %.";

	//// 获取当前屏幕对象
	//QScreen *screen = QGuiApplication::primaryScreen();
	//if (const QWindow *windowHandle = this->windowHandle()) {
	//	screen = windowHandle->screen();  // 获取窗口所属屏幕
	//}

	//if (!screen) return;

	//// 获取当前窗口内容的截图
	//QPixmap pixmap = screen->grabWindow(this->winId());
	//figName = "./Data/Fig_" + strCMID + strBKID + ".png";
	//pixmap.save(figName);
}

void MainWindow::on_pushButton_calUniformity_clicked()
{
	m_BK->GenUniformityLUT();
	cv::Mat_<qreal> uMat = m_BK->GetUniformity();
	QVector<qreal> uStat = m_BK->GetUStatistics(); // max, min, mean, stdDev
	//qreal uMax = uStat[0];
	//qreal uMin = uStat[1];
	//qreal uMean = uStat[2];
	qreal uSD = uStat[3];
	ui->lineEdit_stdDev->setText(QString::number(uSD * 100, 'f', 2) + " %");

	ShowImage(uMat);
	QPixmap aPixmap = *(ui->label_floodmap->pixmap());
	QPainter aPainter(&aPixmap);
	aPainter.setPen(QPen(Qt::black, 1));
	aPainter.setFont(QFont("Arial", 10));
	qreal width = 1.0 * nPixel / nCrystal;
	for (int iRow = 0; iRow < nCrystal; ++iRow)
	{
		for (int iCol = 0; iCol < nCrystal; ++iCol)
		{
			qreal iPar = 1.0 / uMat(iRow, iCol);
			QRect rect(iCol * width, iRow * width, width, width);
			aPainter.drawText(rect, Qt::AlignCenter, QString::number(iPar, 'f', 1));
		}
	}
	ui->label_floodmap->setPixmap(aPixmap);

	// 保存图片
	QString strCMID = ui->lineEdit_CMID->text().rightJustified(2, '0');
	QString strBKID = ui->lineEdit_BKID->text().rightJustified(2, '0');

	QImage image = aPixmap.toImage();
	QString figName = "./Data/Uniformity_" + strCMID + "_" + strBKID + ".png";
	image.save(figName);

	/*************************
	// 保存文件
	QFile outfile(currentPath + QStringLiteral("查找表") + "/ULUT");
	if (outfile.open(QIODevice::WriteOnly))
	{
		QDataStream out(&outfile);

		for (int iRow = 0; iRow < nCrystal; ++iRow)
		{
			for (int iCol = 0; iCol < nCrystal; ++iCol)
			{
				qreal c = 1. / uMat(iRow, iCol);
				out << c;
			}
		}
		outfile.close();
	}
	else
	{
		qDebug() << QString::fromLocal8Bit("均匀性查找表生成失败。");
	}
	**************************/

	// 获取当前屏幕对象
	QScreen* screen = QGuiApplication::primaryScreen();
	if (const QWindow* windowHandle = this->windowHandle())
	{
		screen = windowHandle->screen();  // 获取窗口所属屏幕
	}

	if (!screen) return;

	// 获取当前窗口内容的截图
	QPixmap pixmap = screen->grabWindow(this->winId());
	figName = "./Data/Fig_" + strCMID + "_" + strBKID + ".png";
	pixmap.save(figName);
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

void MainWindow::UpdateProgressBar(int pos)
{
	ui->progressBar_readinData->setValue(pos);
}

void MainWindow::on_pushButton_crystalES_clicked()
{
	imgFlag = 3;
	EWLeftLine->replace(0, -1, 0);
	EWLeftLine->replace(1, -1, 1);

	EWRightLine->replace(0, -1, 0);
	EWRightLine->replace(1, -1, 1);

	ShowRecEHist();
}

void MainWindow::ShowRecEHist()
{
	int rowID = ui->lineEdit_rowID->text().toInt();
	int colID = ui->lineEdit_colID->text().toInt();

	int crystalID = rowID * nCrystal + colID;
	auto iHist = m_BK->GetCrystal(crystalID)->GetRecEHist();
	QVector<QPointF> points;
	for (int i = 0; i < recE_nBins; ++i)
	{
		qreal x = iHist->GetBinCenter(i);
		qreal y = iHist->GetBinContent(i);
		points.append(QPointF(x, y));
	}
	eHistLine->replace(points);

	peakLine->replace(0, -1, 0);
	peakLine->replace(1, -1, 1);

	axisX->setRange(recE_min, recE_max);
	axisX->applyNiceNumbers();
	axisY->setRange(0, iHist->GetPeak().y() * 1.1);

	QPixmap pixmap = chartView->grab();
	ui->label_floodmap->setPixmap(pixmap);
}

void MainWindow::on_pushButton_report_clicked()
{
	QString fName = "Report.txt";
	QFile file(fName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
	{
		qDebug() << "Fail to open file, please check file: " << fName;
		return;
	}

	QTextStream out(&file);
	out << "CMID = " << ui->lineEdit_CMID->text() << "    ";
	out << "BKID = " << ui->lineEdit_BKID->text() << "\n";
	out << "PVR = " << ui->lineEdit_PVR->text() << "    ";
	out << "FWHM = " << ui->lineEdit_FWHM->text() << "\n";

	file.close();
}

void MainWindow::on_FloodmapClosed(cv::Mat_<qreal> I, cv::Mat_<cv::Vec2w> pt)
{
	m_BK->SetMap(I);
	m_BK->SetPeakTable(pt);
	ShowPeaks(I, pt);
}

void MainWindow::ShowMessage(QString title, QString content)
{
	QMessageBox::information(this, title, content);
}