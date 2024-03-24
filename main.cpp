#include "Mainwindow.h"

#include <QApplication>
#include <QString>
#include <QDir>
#include <QMap>
#include <QFile>

#include "Parameters.h"


int nCM;
int nBK;
int nPixel;
int nCrystal;
int crystalNum;

qreal factor; // 调整点阵大小
int bias;  // 调整点阵位置

// choose central area to analysis
int xmin; //int xmin = 0;
int xmax; //int xmax = 512;
int ymin; //int ymin = 0;
int ymax; //int ymax = 512;

QString segmentMethod;

qreal EW_width;
int ADC_min;
int ADC_max; // ADC channel value
int ADC_nBins;
int ADC_binWidth;
int ADC_cutValue; // 将小于该值的bin置为0，防止寻峰错误

int recE_min;
int recE_max; // keV
int recE_nBins;
int recE_binWidth;
int recE_cutValue; // 将小于该值的bin置为0，防止寻峰错误

QString currentPath;
QString fName_LUT_P;
QString fName_LUT_E;
QString fName_LUT_U;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDir dir(QCoreApplication::applicationDirPath()); // 获取当前程序目录
    currentPath = dir.absolutePath() + "/";

    QString fullName = currentPath + "config.par";
    QFile file(fullName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "unopen parameter file!";
        return 1;
    }

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
            qDebug() << "Invalid format!";
        }
    }

    nCM = pars["nCM"].toInt();
    nBK = pars["nBK"].toInt();
    nPixel = pars["nPixel"].toInt();
    nCrystal = pars["nCrystal"].toInt();
    crystalNum = nCrystal * nCrystal;

    factor = pars["enlarge"].toDouble(); // 调整点阵大小
    bias = pars["bias"].toInt();  // 调整点阵位置

    // choose central area to analysis
    xmin = pars["xmin"].toInt(); //int xmin = 0;
    xmax = pars["xmax"].toInt(); //int xmax = 512;
    ymin = pars["ymin"].toInt(); //int ymin = 0;
    ymax = pars["ymax"].toInt(); //int ymax = 512;

    segmentMethod = pars["segmentMethod"];

    EW_width = pars["EW_width"].toDouble();
    ADC_min = pars["ADC_min"].toInt();
    ADC_max = pars["ADC_max"].toInt(); // ADC channel value
    ADC_nBins = pars["ADC_nBins"].toInt();
    ADC_binWidth = (ADC_max-ADC_min)/ADC_nBins;
    ADC_cutValue = pars["ADC_cutValue"].toInt();

    recE_min = pars["recE_min"].toInt();
    recE_max = pars["recE_max"].toInt(); // keV
    recE_nBins = pars["recE_nBins"].toInt();
    recE_binWidth = (recE_max-recE_min)/recE_nBins;
    recE_cutValue = pars["recE_cutValue"].toInt();

    fName_LUT_P = currentPath + pars["Position LUT"];
    fName_LUT_E = currentPath + pars["Energy LUT"];
    fName_LUT_U = currentPath + pars["Uniformity LUT"];

    //判断当前目录中是否存在文件夹"Data/"，如果不存在则创建一个
    QString directoryName = "Data";

    // 拼接目标文件夹的路径
    QString targetDirectoryPath = dir.absoluteFilePath(directoryName);

    // 检查文件夹是否存在，如果不存在则创建文件夹
    if (!dir.exists(directoryName))
    {
        if (dir.mkdir(directoryName))
        {
            qDebug() << "Data directory created successfully: " << targetDirectoryPath;
        }
        else
        {
            qDebug() << "Failed to create data directory: " << targetDirectoryPath;
        }
    }

    MainWindow window;
    window.setWindowTitle("CPM: crystal position map program");
    window.show();
    return app.exec();
}
