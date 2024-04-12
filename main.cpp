#include <QApplication>
#include <QString>
#include <QDir>
#include <QMap>
#include <QFile>
#include <QtDebug>
#include <QScreen>

#include "Parameters.h"
#include "ParameterForm.h"


int nCM;
int nBK;
int nPixel;
int nCrystal;
int crystalNum;

qreal enlarge;
int bias;

int xmin;
int xmax;
int ymin;
int ymax;

qreal EW_width;

SegmentMethod segMethod;

qreal peakE;

int ADC_min;
int ADC_max;
int ADC_nBins;
qreal ADC_binWidth;
int ADC_cutValue;

qreal recE_min;
qreal recE_max;
int recE_nBins;
qreal recE_binWidth;
qreal recE_cutValue;

QString currentPath;
QString fName_LUT_P;
QString fName_LUT_E;
QString fName_LUT_U;


void myMessageOutput(QtMsgType type,
                     const QMessageLogContext &context,
                     const QString &msg)
{
    QByteArray localMsg = msg.toUtf8();
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


    // readin parameters
    QString fName = currentPath + ".config.par";
    QFile file(fName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);

        QMap<QString, QString> pars;

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
            }
            else
            {
                qWarning() << "Invalid format!";
            }
        }

        nCM = pars["nCM"].toInt();
        nBK = pars["nBK"].toInt();
        nPixel = pars["nPixel"].toInt();
        nCrystal = pars["nCrystal"].toInt();
        crystalNum = nCrystal * nCrystal;

        enlarge = pars["enlarge"].toDouble(); // 调整点阵大小
        bias = pars["bias"].toInt();  // 调整点阵位置

        xmin = pars["xmin"].toInt();
        xmax = pars["xmax"].toInt();
        ymin = pars["ymin"].toInt();
        ymax = pars["ymax"].toInt();

        EW_width = pars["EW_width"].toDouble();

        switch (pars["segmentMethod"].toInt())
        {
        case 1:
            segMethod = SegmentMethod::SVD;
            break;
        case 2:
            segMethod = SegmentMethod::FindMaximum;
            break;
        default:
            qDebug() << "SegmentMethod parameter is wrong.";
            break;
        }

        peakE = pars["peakE"].toDouble();

        ADC_min = pars["ADC_min"].toInt();
        ADC_max = pars["ADC_max"].toInt(); // ADC channel value
        ADC_nBins = pars["ADC_nBins"].toInt();
        ADC_binWidth = 1.0*(ADC_max-ADC_min)/ADC_nBins;
        ADC_cutValue = pars["ADC_cutValue"].toInt();

        recE_min = pars["recE_min"].toDouble();
        recE_max = pars["recE_max"].toDouble(); // keV
        recE_nBins = pars["recE_nBins"].toInt();
        recE_binWidth = (recE_max-recE_min)/recE_nBins;
        recE_cutValue = pars["recE_cutValue"].toDouble();

        fName_LUT_P = pars["Position LUT"];
        fName_LUT_E = pars["Energy LUT"];
        fName_LUT_U = pars["Uniformity LUT"];

        file.close();
    }
    else
    {
        // 如果不存在参数文件，则直接设默认值
        nCM = 20;
        nBK = 5;
        nPixel = 512;
        nCrystal = 26;
        crystalNum = nCrystal*nCrystal;

        enlarge = 1.06;
        bias = 58;

        xmin = 0;
        xmax = 512;
        ymin = 0;
        ymax = 512;

        EW_width = 0.25;

        segMethod = SegmentMethod::SVD;

        peakE = 511;

        ADC_min = 0;
        ADC_max = 60000;
        ADC_nBins = 600;
        ADC_binWidth = (ADC_max - ADC_min) / ADC_nBins;
        ADC_cutValue = 15000;

        recE_min = 0;
        recE_max = 1600;
        recE_nBins = 800;
        recE_binWidth = (recE_max - recE_min) / recE_nBins;
        recE_cutValue = 200;

        fName_LUT_P = "LUT_Position.bin";
        fName_LUT_E = "LUT_Energy.bin";
        fName_LUT_U = "LUT_Uniformity.bin";
    }

    ParameterForm* parWindow = new ParameterForm();
    parWindow->setWindowTitle("Parameters Setup Window");
    parWindow->move(50, 50);
    parWindow->show();

    return app.exec();
}
