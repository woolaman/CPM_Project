#include "Mainwindow.h"
#include "ParameterForm.h"

#include <QApplication>
#include <QString>
#include <QDir>
#include <QMap>
#include <QFile>
#include <QtDebug>

#include "Parameters.h"


int nCM = 20;
int nBK = 5;
int nPixel = 512;
int nCrystal = 26;
int crystalNum = nCrystal*nCrystal;

qreal enlargeFactor = 1.06;
int bias = 58;

int xmin = 0;
int xmax = 512;
int ymin = 0;
int ymax = 512;

qreal EW_width = 0.25;

int ADC_min = 0;
int ADC_max = 60000;
int ADC_nBins = 600;
qreal ADC_binWidth = (ADC_max - ADC_min) / ADC_nBins;
int ADC_cutValue = 15000;

qreal recE_min = 0;
qreal recE_max = 1600;
int recE_nBins = 800;
qreal recE_binWidth = (recE_max - recE_min) / recE_nBins;
qreal recE_cutValue = 200;

QString currentPath;
QString fName_LUT_P = "LUT_Position.bin";
QString fName_LUT_E = "LUT_Energy.bin";
QString fName_LUT_U = "LUT_Uniformity.bin";

qreal peakE = 511;

SegmentMethod segMethod = SegmentMethod::SVD;


void myMessageOutput(QtMsgType type,
                     const QMessageLogContext &context,
                     const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type)
    {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n",
                localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n",
                localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n",
                localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n",
                localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n",
                localMsg.constData(), file, context.line, function);
        break;
    }
}


int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);

    QApplication app(argc, argv);

    // 获取当前程序目录
    QDir dir(QCoreApplication::applicationDirPath());
    currentPath = dir.absolutePath() + "/";
    qInfo() << "Current path = " << currentPath;

    //判断当前目录中是否存在文件夹"Data/"，如果不存在则创建一个
    QString directoryName = "Data";

    // 拼接目标文件夹的路径
    QString targetDirectoryPath = dir.absoluteFilePath(directoryName);

    // 检查文件夹是否存在，如果不存在则创建文件夹
    if (!dir.exists(directoryName))
    {
        if (dir.mkdir(directoryName))
        {
            qDebug() << "Data directory created successfully: " <<
                targetDirectoryPath;
        }
        else
        {
            qCritical() << "Failed to create data directory: " <<
                targetDirectoryPath;
        }
    }

    ParameterForm parWindow;
    parWindow.setWindowTitle("Parameters Setup Window");
    parWindow.show();





    MainWindow mainWindow;
    mainWindow.setWindowTitle("CPM: crystal position map program");
    mainWindow.show();

    return app.exec();
}
